//
// Created by Jerry Chou on 2022/3/26.
//

#include "ComparingExpressionNodeGenerator.hpp"

ComparingExpressionNodeGenerator::ComparingExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ComparingExpressionNodeGenerator::Parse() {
    AST Left = BinaryMoveExpressionNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::LessThan and L.LastToken.Kind != Lexer::TokenKind::MoreThan and
        L.LastToken.Kind != Lexer::TokenKind::LessThanOrEqual and
        L.LastToken.Kind != Lexer::TokenKind::MoreThanOrEqual) {
        return Left;
    }
    AST Operator = {AST::TreeType::Operator, L.LastToken};
    L.Scan();
    AST Right = ComparingExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a rvalue expression.");
    }
    return {AST::TreeType::ComparingExpression, {Left, Operator, Right}};
}
