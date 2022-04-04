//
// Created by Jerry Chou on 2022/4/4.
//

#ifndef XLANG2_RETURNSTATEMENTNODEGENERATOR_HPP
#define XLANG2_RETURNSTATEMENTNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "ExpressionNodeGenerator.hpp"

class ReturnStatementNodeGenerator : BaseGenerator {
public:
    explicit ReturnStatementNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_RETURNSTATEMENTNODEGENERATOR_HPP
