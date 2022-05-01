//
// Created by Jerry Chou on 2022/5/1.
//

#include "WhileStatementNodeGenerator.hpp"

WhileStatementNodeGenerator::WhileStatementNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST WhileStatementNodeGenerator::Parse() {
    if (L.LastToken.Kind != Lexer::TokenKind::ReservedWords or L.LastToken.Value != L"while") {
        return {};
    }
    L.Scan();

    if(L.LastToken.Kind != Lexer::TokenKind::LeftParentheses) {
        MakeException(L"WhileStatementNodeGenerator : Expected a '(' after 'for'.");
    }
    L.Scan();

    AST ConditionExpr = ExpressionNodeGenerator(L).Parse();
    if (ConditionExpr.IsNotMatchNode() or ConditionExpr.Type == AST::TreeType::AssignmentExpression) {
        MakeException(L"WhileStatementNodeGenerator : Expected a lvalue expression after '('.");
    }

    if (L.LastToken.Kind != Lexer::TokenKind::RightParentheses) {
        MakeException(L"WhileStatementNodeGenerator : Expected a ')' after lvalue expression.");
    }
    L.Scan();

    AST CodeBlockStmt = CodeBlockNodeGenerator(L).Parse();
    if (CodeBlockStmt.IsNotMatchNode()) {
        MakeException(L"WhileStatementNodeGenerator : Expected a code block statement after ')'.");
    }

    return {AST::TreeType::WhileStatement, {ConditionExpr, CodeBlockStmt}};
}
