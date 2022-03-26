//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_MULTIPLICATIONEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_MULTIPLICATIONEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class MultiplicationExpressionNodeGenerator : BaseGenerator {
public:
    explicit MultiplicationExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_MULTIPLICATIONEXPRESSIONNODEGENERATOR_HPP
