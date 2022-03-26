//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_EXPRESSIONNODEGENERATOR_HPP
#define XLANG2_EXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class ExpressionNodeGenerator : BaseGenerator {
public:
    explicit ExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_EXPRESSIONNODEGENERATOR_HPP
