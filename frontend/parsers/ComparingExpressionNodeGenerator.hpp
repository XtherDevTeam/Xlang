//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_COMPARINGEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_COMPARINGEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "BinaryMoveExpressionNodeGenerator.hpp"

class ComparingExpressionNodeGenerator : BaseGenerator {

public:
    AST Parse() override;

    explicit ComparingExpressionNodeGenerator(Lexer &L);
};


#endif //XLANG2_COMPARINGEXPRESSIONNODEGENERATOR_HPP
