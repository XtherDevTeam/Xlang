//
// Created by Jerry Chou on 2022/4/4.
//

#ifndef XLANG2_STATEMENTNODEGENERATOR_HPP
#define XLANG2_STATEMENTNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "VariableDefinitionNodeGenerator.hpp"
#include "FunctionDefinitionNodeGenerator.hpp"
#include "ReturnStatementNodeGenerator.hpp"
#include "ContinueStatementNodeGenerator.hpp"
#include "BreakStatementNodeGenerator.hpp"
#include "CodeBlockNodeGenerator.hpp"
#include "ExpressionNodeGenerator.hpp"

#include "IfStatementNodeGenerator.hpp"
#include "ForStatementNodeGenerator.hpp"
#include "WhileStatementNodeGenerator.hpp"

class StatementNodeGenerator : BaseGenerator {
public:
    explicit StatementNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_STATEMENTNODEGENERATOR_HPP
