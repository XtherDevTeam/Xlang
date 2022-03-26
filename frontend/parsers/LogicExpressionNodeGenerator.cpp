//
// Created by Jerry Chou on 2022/3/26.
//

#include "LogicExpressionNodeGenerator.hpp"

LogicExpressionNodeGenerator::LogicExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST LogicExpressionNodeGenerator::Parse() {
    AST Left = BinaryExpressionNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::LogicAnd and L.LastToken.Kind != Lexer::TokenKind::LogicOr) {
        return Left;
    }
    AST Operator = {AST::TreeType::Operator, L.LastToken};
    L.Scan();
    AST Right = LogicExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a rvalue expression.");
    }
    return {AST::TreeType::LogicExpression, {Left, Operator, Right}};
}
