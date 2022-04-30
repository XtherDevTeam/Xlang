//
// Created by Jerry Chou on 2022/4/30.
//

#include "ForStatementNodeGenerator.hpp"

ForStatementNodeGenerator::ForStatementNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ForStatementNodeGenerator::Parse() {
    if (L.LastToken.Kind != Lexer::TokenKind::ReservedWords or L.LastToken.Value != L"for") {
        return {};
    }
    L.Scan();
    if (L.LastToken.Kind != Lexer::TokenKind::LeftParentheses) {
        MakeException(L"ForStatementNodeGenerator : Expected a left parentheses after 'for'.");
    }
    L.Scan();

    AST InitialStatement = StatementNodeGenerator(L).Parse();
    if (InitialStatement.IsNotMatchNode()) {
        MakeException(L"ForStatementNodeGenerator : Expected a statement after parentheses.");
    }
    if (L.LastToken.Kind != Lexer::TokenKind::Semicolon) {
        MakeException(L"ForStatementNodeGenerator : Expected a semicolon after statement.");
    }
    L.Scan();

    AST ConditionExpr = ExpressionNodeGenerator(L).Parse();
    if (ConditionExpr.IsNotMatchNode()) {
        MakeException(L"ForStatementNodeGenerator : Expected a expression after semicolon.");
    }
    if (L.LastToken.Kind != Lexer::TokenKind::Semicolon) {
        MakeException(L"ForStatementNodeGenerator : Expected a semicolon after expression.");
    }
    L.Scan();

    AST LoopEndStatement = StatementNodeGenerator(L).Parse();
    if (LoopEndStatement.IsNotMatchNode()) {
        MakeException(L"ForStatementNodeGenerator : Expected a statement after semicolon.");
    }
    if (L.LastToken.Kind != Lexer::TokenKind::RightParentheses) {
        MakeException(L"ForStatementNodeGenerator : Expected a right parentheses after statement.");
    }
    L.Scan();

    AST LoopCodeBlock = CodeBlockNodeGenerator(L).Parse();
    if (LoopCodeBlock.IsNotMatchNode()) {
        MakeException(L"ForStatementNodeGenerator : Expected a code block after right parentheses.");
    }

    return {AST::TreeType::ForStatement, {InitialStatement, ConditionExpr, LoopEndStatement, LoopCodeBlock}};
}
