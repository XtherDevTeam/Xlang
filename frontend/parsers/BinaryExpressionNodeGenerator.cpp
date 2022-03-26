//
// Created by Jerry Chou on 2022/3/26.
//

#include "BinaryExpressionNodeGenerator.hpp"

BinaryExpressionNodeGenerator::BinaryExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST BinaryExpressionNodeGenerator::Parse() {
    AST Left = EqualExpressionNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::BinaryXOR and L.LastToken.Kind != Lexer::TokenKind::BinaryOr and L.LastToken.Kind != Lexer::TokenKind::BinaryAnd) {
        return Left;
    }
    AST Operator = {AST::TreeType::Operator, L.LastToken};
    L.Scan();
    AST Right = BinaryExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a rvalue expression.");
    }
    return {AST::TreeType::BinaryExpression, {Left, Operator, Right}};
}
