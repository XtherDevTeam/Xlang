//
// Created by Jerry Chou on 2022/3/26.
//

#include "BinaryMoveExpressionNodeGenerator.hpp"

BinaryMoveExpressionNodeGenerator::BinaryMoveExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST BinaryMoveExpressionNodeGenerator::Parse() {
    AST Left = AdditionExpressionNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::BinaryLeftMove and L.LastToken.Kind != Lexer::TokenKind::BinaryRightMove) {
        return Left;
    }
    AST Operator = {AST::TreeType::Operator, L.LastToken};
    L.Scan();
    AST Right = BinaryMoveExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a rvalue expression.");
    }
    return {AST::TreeType::BinaryMoveExpression, {Left, Operator, Right}};
}
