//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_BINARYMOVEEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_BINARYMOVEEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "AdditionExpressionNodeGenerator.hpp"

class BinaryMoveExpressionNodeGenerator : BaseGenerator {
public:
    explicit BinaryMoveExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_BINARYMOVEEXPRESSIONNODEGENERATOR_HPP
