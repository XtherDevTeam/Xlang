//
// Created by Jerry Chou on 2022/3/26.
//

#include "MultiplicationExpressionNodeGenerator.hpp"

MultiplicationExpressionNodeGenerator::MultiplicationExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST MultiplicationExpressionNodeGenerator::Parse() {
    AST Left = IncrementExpressionNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        Left = DecrementExpressionNodeGenerator(L).Parse();
        if (Left.IsNotMatchNode()) {
            Rollback();
            return {};
        }
    }
    if (L.LastToken.Kind != Lexer::TokenKind::Asterisk and L.LastToken.Kind != Lexer::TokenKind::PercentSign and
        L.LastToken.Kind != Lexer::TokenKind::Slash) {
        return Left;
    }
    AST Operator = {AST::TreeType::Operator, L.LastToken};
    L.Scan();
    AST Right = MultiplicationExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a rvalue expression.");
    }
    return {AST::TreeType::MultiplicationExpression, {Left, Operator, Right}};
}
