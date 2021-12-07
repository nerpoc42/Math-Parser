#include <cstdlib>
#include <cmath>
#include <cstring>
#include "math_parse.h"

//////////////////////////////////////////////////////////

EXPR_RES parse_begin(char *&expr, long double &n1);
EXPR_RES parse_half(char *&expr, long double &n1, char op1);

//////////////////////////////////////////////////////////

typedef long double (*math_func)(long double x);

static bool EQUAL_EXISTS;

int fact(int num) {
    if (num <= 1) return 1;
    return num * fact(num - 1);
}

bool substrcmp(const char* str, const char* substr, char*& end) {
    size_t i = 0;
    while (substr[i]) {
        if (substr[i] != str[i])
            return false;
        ++i;
    }
    end += i;
    return true;
}

//////////////////////////////////////////////////////////

EXPR_RES read_function(char *&expr, long double &number) {
    math_func func;
    if (substrcmp(expr, "cos", expr)) func = cosl;
    else if (substrcmp(expr, "sin", expr)) func = sinl;
    else if (substrcmp(expr, "tan", expr)) func = tanl;
    else if (substrcmp(expr, "arctan", expr)) func = atanl;
    else if (substrcmp(expr, "arccos", expr)) func = acosl;
    else if (substrcmp(expr, "log", expr)) func = logl;
    else if (substrcmp(expr, "exp", expr)) func = expl;
    else return NO_EXPR;

    // Can't be spaces here

    if (*expr != '(')
        return MALFORMED_EXPR;
    ++expr;


    long double in_number;

    // Has to be malformed expression (cannot recognise ), and symbol must be closing parentheses
    EXPR_RES res = parse_begin(expr, in_number);
    if (res != MALFORMED_EXPR || *expr != ')')
        return MALFORMED_EXPR;

    ++expr;

    number = func(in_number);
    return VALID_EXPR;
}

//////////////////////////////////////////////////////////

// Reads number from the math expression,
// format: <sign><number><optional factorial sign>, number can be subexpression in parentheses to which parse() is called
EXPR_RES read_num(char *&expr, long double &number) {
    int sign = 1;
    bool has_sign = false;

    while (*expr == ' ') ++expr; // Skip whitespace

    if (*(expr) == '+') { // Handle number sign
        sign = 1;
        has_sign = true;
        ++expr;

        while (*expr == ' ') ++expr; // Skip whitespace

    } else if (*(expr) == '-') {
        sign = -1;
        has_sign = -1;
        ++expr;

        while (*expr == ' ') ++expr; // Skip whitespace

    }

    EXPR_RES res;
    res = read_function(expr, number);
    if (res != NO_EXPR)
        return res;

    if (*(expr) == '(') { // Handle parentheses (subexpression)
        ++expr;

        // Has to be malformed expression (cannot recognise ), and symbol must be closing parentheses
        res = parse_begin(expr, number);
        if (res != MALFORMED_EXPR || *expr != ')')
            return MALFORMED_EXPR;

        number *= sign;
        ++expr;
        return VALID_EXPR;
    }

    char *num_end;
    number = strtold(expr, &num_end);

    if (num_end == expr)
        return has_sign ? MALFORMED_EXPR : NO_EXPR;

    expr = num_end;

    while (*expr == ' ') ++expr; // Skip whitespace

    if (*expr == '!') { // Handle factorial
        if (ceill(number) != number) return MALFORMED_EXPR;
        number = fact((int) number);
        ++expr;
    }

    number *= sign;
    return VALID_EXPR;
}

// Reads an operator, operators: +-*/%=^
EXPR_RES read_op(char *&expr, char &op) {
    while (*expr == ' ') ++expr; // Skip whitespace

    op = *expr;

    if (op == 0)
        return NO_EXPR;

    if (op != '+' && op != '-' && op != '*' && op != '/' && op != '%' && op != '=' && op != '^') {
        return MALFORMED_EXPR;
    }

    ++expr;

    return VALID_EXPR;
}

// Operator precedence, higher more priority
// Assuming valid operator gotten from read_op
int precedence(const char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
        case '%':
            return 2;
        case '^':
            return 3;
        case '(':
            return 4;
        case '=':
            return 0;
        default:
            return -1;
    }
}

// Apply operator on two numbers saving result on the first number
EXPR_RES exec_op(long double &n1, const long double n2, const char op) {
    switch (op) {
        case '+':
            n1 += n2;
            break;
        case '-':
            n1 -= n2;
            break;
        case '*':
            n1 *= n2;
            break;
        case '/':
            if (n2 == 0)
                return MALFORMED_EXPR;

            n1 /= n2;
            break;
        case '%':
            // Makes sure it's an integer (even though it's float)
            if (ceill(n1) != n1 || ceill(n2) != n2)
                return MALFORMED_EXPR;

            n1 = (int) n1 % (int) n2;
            break;
        case '^':
            n1 = powl(n1, n2);
            break;
        case '=':
            // Prevents equal from reoccurring twice in the expression
            if (EQUAL_EXISTS) return MALFORMED_EXPR;

            n1 -= n2;
            EQUAL_EXISTS = true;
            break;
        default:
            return MALFORMED_EXPR;
    }
    return VALID_EXPR;
}

// Parses other half of the subexpression, n1 and op1 is given
/* Algorithm:
n1 = first number, op1 = first operator, n2 = second number, op2 = second operator
n1, op1 given

if second number not exist end parsing
otherwise check precedence of operators:
	if op2 > op1: parse right half as n2, and apply op1 to n1 and new n2
	else: apply op1 to n1 and n2 (as n1) and parse right half, with new n1 and op2
*/
EXPR_RES parse_half(char *&expr, long double &n1, const char op1) {
    long double n2;
    char op2;
    EXPR_RES res;

    // 1 number 1 operator
    if (read_num(expr, n2) != VALID_EXPR)
        return MALFORMED_EXPR;

    // 2 numbers 1 operator
    res = read_op(expr, op2);
    if (res == NO_EXPR)
        return exec_op(n1, n2, op1);
    else if (res == MALFORMED_EXPR) {
        exec_op(n1, n2, op1);
        return MALFORMED_EXPR;
    }

    // 2 numbers 2 operators
    if (precedence(op2) > precedence(op1)) {
        // Calculates right side: forms n2 by applying higher precedence operators on the right recursively
        if (parse_half(expr, n2, op2) != VALID_EXPR)
            return MALFORMED_EXPR;

        // Calculate left side: apply op1 to n1 and newly formed n2
        return exec_op(n1, n2, op1);

    } else {
        // Calculate left side: forms n1 by applying operator op1 to n1 and n2
        if (exec_op(n1, n2, op1) != VALID_EXPR)
            return MALFORMED_EXPR;

        // Calculate right side: parse further to the right recursively with newly formed n1 and op2
        return parse_half(expr, n1, op2);
    }
}

// Parse subexpression: get n1 and op1, and call parse half with n1 and op2
EXPR_RES parse_begin(char *&expr, long double &n1) {
    char op1;
    EXPR_RES res;

    res = read_num(expr, n1);
    if (res != VALID_EXPR)
        return res;

    res = read_op(expr, op1);
    if (res == NO_EXPR)
        return VALID_EXPR;
    else if (res == MALFORMED_EXPR)
        return MALFORMED_EXPR;

    return parse_half(expr, n1, op1); // Parse other half
}

// Wrapper for parse_begin.
EXPR_RES parse(char *expr, long double &n1) {
    EQUAL_EXISTS = false;

    return parse_begin(expr, n1);
}
