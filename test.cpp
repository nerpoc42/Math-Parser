//
// Created by np on 2021-12-07.
//

#include <iostream>
#include "math_parse.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
const size_t MAX_EXPR_LENGTH = 1024;

int main() {

    char expr[MAX_EXPR_LENGTH];
    long double rez;

    std::cout << "Įveskite lygtį ar bet kokį skaičiavimą: " << std::endl;

    while (true) {
        std::cin.getline(expr, MAX_EXPR_LENGTH);
        if (!parse(expr, rez)) {
            std::cerr << "Neteisinga lygtis ar skaičiavimas" << std::endl;
        }
        else
            std::cout << rez << std::endl;
    }
}
#pragma clang diagnostic pop