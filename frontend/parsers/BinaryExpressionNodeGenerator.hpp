//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_BINARYEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_BINARYEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "EqualExpressionNodeGenerator.hpp"

class BinaryExpressionNodeGenerator : BaseGenerator {
public:
    explicit BinaryExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_BINARYEXPRESSIONNODEGENERATOR_HPP
