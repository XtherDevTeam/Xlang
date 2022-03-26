//
// Created by Jerry Chou on 2022/3/26.
//

#include "EqualExpressionNodeGenerator.hpp"

EqualExpressionNodeGenerator::EqualExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST EqualExpressionNodeGenerator::Parse() {
    AST Left = ComparingExpressionNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::Equal and L.LastToken.Kind != Lexer::TokenKind::NotEqual) {
        return Left;
    }
    AST Operator = {AST::TreeType::Operator, L.LastToken};
    L.Scan();
    AST Right = EqualExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a rvalue expression.");
    }
    return {AST::TreeType::EqualExpression, {Left, Operator, Right}};
}
