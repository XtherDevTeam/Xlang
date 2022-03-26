//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_ADDITIONEXPRESSIONNODEGRAMMAR_HPP
#define XLANG2_ADDITIONEXPRESSIONNODEGRAMMAR_HPP

#include "BaseGenerator.hpp"

class AdditionExpressionNodeGrammar : BaseGenerator {
public:
    explicit AdditionExpressionNodeGrammar(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_ADDITIONEXPRESSIONNODEGRAMMAR_HPP
