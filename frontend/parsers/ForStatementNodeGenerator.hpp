//
// Created by Jerry Chou on 2022/4/30.
//

#ifndef XLANG2_FORSTATEMENTNODEGENERATOR_HPP
#define XLANG2_FORSTATEMENTNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "StatementNodeGenerator.hpp"
#include "ExpressionNodeGenerator.hpp"
#include "CodeBlockNodeGenerator.hpp"

class ForStatementNodeGenerator : BaseGenerator {
public:
    explicit ForStatementNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_FORSTATEMENTNODEGENERATOR_HPP
