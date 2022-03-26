//
// Created by Jerry Chou on 2022/3/26.
//

#include "AdditionExpressionNodeGenerator.hpp"

AdditionExpressionNodeGenerator::AdditionExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST AdditionExpressionNodeGenerator::Parse() {
    AST Left = MultiplicationExpressionNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::Plus and L.LastToken.Kind != Lexer::TokenKind::Minus) {
        return Left;
    }
    AST Operator = {AST::TreeType::Operator, L.LastToken};
    L.Scan();
    AST Right = AdditionExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a rvalue expression.");
    }
    return {AST::TreeType::AdditionExpression, {Left, Operator, Right}};
}
