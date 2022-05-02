//
// Created by Jerry Chou on 2022/5/2.
//

#include "GlobalStatementNodeGenerator.hpp"

GlobalStatementNodeGenerator::GlobalStatementNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST GlobalStatementNodeGenerator::Parse() {
    AST Result = VariableDefinitionNodeGenerator(L).Parse();
    if (Result.Type == AST::TreeType::VariableDeclaration)
        return Result;

    Result = FunctionDefinitionNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    return {};
}
