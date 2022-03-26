//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_AdditionExpressionNodeGenerator_HPP
#define XLANG2_AdditionExpressionNodeGenerator_HPP

#include "BaseGenerator.hpp"
#include "MultiplicationExpressionNodeGenerator.hpp"

class AdditionExpressionNodeGenerator : BaseGenerator {
public:
    explicit AdditionExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_AdditionExpressionNodeGenerator_HPP
