#ifndef MATH_PARSER_MATH_PARSE_H
#define MATH_PARSER_MATH_PARSE_H

enum EXPR_RES {
    NO_EXPR = -1,
    MALFORMED_EXPR = 0,
    VALID_EXPR = 1
};

EXPR_RES parse(char *expr, long double &n1);

#endif //MATH_PARSER_MATH_PARSE_H
