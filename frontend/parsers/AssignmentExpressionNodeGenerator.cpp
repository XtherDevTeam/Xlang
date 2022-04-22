//
// Created by Jerry Chou on 2022/3/26.
//

#include "AssignmentExpressionNodeGenerator.hpp"

AssignmentExpressionNodeGenerator::AssignmentExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST AssignmentExpressionNodeGenerator::Parse() {
    AST Left = MemberExpressionNodeGenerator(L).Parse();
    if (Left.IsNotMatchNode()) {
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::AssignSign and
        L.LastToken.Kind != Lexer::TokenKind::AdditionAssignment and
        L.LastToken.Kind != Lexer::TokenKind::SubtractionAssignment and
        L.LastToken.Kind != Lexer::TokenKind::MultiplicationAssignment and
        L.LastToken.Kind != Lexer::TokenKind::DivisionAssignment and
        L.LastToken.Kind != Lexer::TokenKind::RemainderAssignment) {
        Rollback();
        return {};
    }
    AST Operator = {AST::TreeType::Operator, L.LastToken};
    L.Scan();
    AST Right = LogicExpressionNodeGenerator(L).Parse();
    if (Right.IsNotMatchNode()) {
        MakeException(L"Expected a rvalue expression.");
    }
    return {AST::TreeType::AssignmentExpression, {Left, Operator, Right}};
}
