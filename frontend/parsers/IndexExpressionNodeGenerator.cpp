//
// Created by Jerry Chou on 2022/3/26.
//

#include "IndexExpressionNodeGenerator.hpp"

IndexExpressionNodeGenerator::IndexExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST IndexExpressionNodeGenerator::Parse() {
    AST Left = IdentifierNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::LeftBracket) {
        return Left;
    }
    L.Scan();
    AST Right = ExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a expression");
    }
    if (L.LastToken.Kind != Lexer::TokenKind::RightBracket) {
        MakeException(L"Expected a right bracket to close a index expression");
    }
    L.Scan();
    Left = {AST::TreeType::IndexExpression, {Left, Right}};

    while (L.LastToken.Kind == Lexer::TokenKind::LeftBracket) {
        L.Scan();
        Right = ExpressionNodeGenerator(L).Parse();
        if (Right.IsNotMatchNode()) {
            MakeException(L"Expected a expression");
        }
        if (L.LastToken.Kind != Lexer::TokenKind::RightBracket) {
            MakeException(L"Expected a right bracket to close a index expression");
        }
        L.Scan();
        Left = {AST::TreeType::IndexExpression, {Left, Right}};
    }
    return Left;
}
