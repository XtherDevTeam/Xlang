//
// Created by Jerry Chou on 2022/4/4.
//

#include "ReturnStatementNodeGenerator.hpp"

ReturnStatementNodeGenerator::ReturnStatementNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ReturnStatementNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::ReservedWords and L.LastToken.Value == L"return") {
        L.Scan();
        AST Expr = ExpressionNodeGenerator(L).Parse();
        if (Expr.IsNotMatchNode()) {
            MakeException(L"return statement expected a expression");
        }
        return {AST::TreeType::ReturnStatement, {Expr}};
    }
    Rollback();
    return {};
}
