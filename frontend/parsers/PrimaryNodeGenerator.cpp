//
// Created by Jerry Chou on 2022/3/25.
//

#include "PrimaryNodeGenerator.hpp"

PrimaryNodeGenerator::PrimaryNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST PrimaryNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::Integer or L.LastToken.Kind == Lexer::TokenKind::Decimal or
        L.LastToken.Kind == Lexer::TokenKind::Boolean or L.LastToken.Kind == Lexer::TokenKind::String) {
        AST Result = {AST::TreeType::Primary, L.LastToken};
        L.Scan(); // prepare for next time.
        return Result;
    } else if (L.LastToken.Kind == Lexer::TokenKind::LeftParentheses) {
        L.Scan();
        AST Expr = ExpressionNodeGenerator(L).Parse();
        if (Expr.IsNotMatchNode() or L.LastToken.Kind != Lexer::TokenKind::RightParentheses) {
            Rollback();
            return {};
        }
        L.Scan();
        if (L.LastToken.Kind == Lexer::TokenKind::TypeCastingSign) {
            L.Scan();
            AST Spec = TypeSpecifierNodeGenerator(L).Parse();
            return {AST::TreeType::TypeCastingExpression, {Expr, Spec}};
        }
        return Expr;
    }
    Rollback();
    return {};
}
