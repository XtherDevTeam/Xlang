//
// Created by Jerry Chou on 2022/3/26.
//

#include "MemberExpressionNodeGenerator.hpp"

MemberExpressionNodeGenerator::MemberExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST MemberExpressionNodeGenerator::Parse() {
    AST Result{};
    Result = FunctionCallingExpressionNodeGenerator(L).Parse();
    if (L.LastToken.Kind != Lexer::TokenKind::Dot) {
        return Result;
    }
    L.Scan();

    while (true) {
        AST Temp = FunctionCallingExpressionNodeGenerator(L).Parse();
        if (Temp.IsNotMatchNode()) break;
        if (L.LastToken.Kind != Lexer::TokenKind::Dot) {
            break;
        }
        L.Scan();

        Result = {AST::TreeType::MemberExpression, {Result, Temp}};
    }
    return Result;
}
