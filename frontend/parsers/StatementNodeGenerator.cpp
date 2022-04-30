//
// Created by Jerry Chou on 2022/4/4.
//

#include "StatementNodeGenerator.hpp"

StatementNodeGenerator::StatementNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST StatementNodeGenerator::Parse() {
    AST Result{};

    Result = VariableDefinitionNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    Result = FunctionDefinitionNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    Result = IfStatementNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    Result = ReturnStatementNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    Result = ContinueStatementNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    Result = BreakStatementNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    Result = CodeBlockNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    Result = ExpressionNodeGenerator(L).Parse();
    if (!Result.IsNotMatchNode())
        return Result;

    return {};
}
