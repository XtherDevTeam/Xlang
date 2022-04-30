//
// Created by Jerry Chou on 2022/4/30.
//

#include "IfStatementNodeGenerator.hpp"

IfStatementNodeGenerator::IfStatementNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST IfStatementNodeGenerator::Parse() {
    if (L.LastToken.Kind != Lexer::TokenKind::ReservedWords or L.LastToken.Value != L"if") {
        return {};
    }
    L.Scan();
    if (L.LastToken.Kind != Lexer::TokenKind::LeftParentheses) {
        Rollback();
        return {};
    }
    L.Scan();
    AST Condition = ExpressionNodeGenerator(L).Parse();
    if (Condition.Type == AST::TreeType::AssignmentExpression) {
        MakeException(L"IfStatementNodeGenerator : Expected a lvalue expression.");
    }
    if (L.LastToken.Kind != Lexer::TokenKind::RightParentheses) {
        Rollback();
        return {};
    }
    L.Scan();
    AST CodeBlock = CodeBlockNodeGenerator(L).Parse();
    if (CodeBlock.IsNotMatchNode()) {
        MakeException(L"IfStatementNodeGenerator : Expected a code block.");
    }

    /* there is an else statement */
    if (L.LastToken.Kind == Lexer::TokenKind::ReservedWords and L.LastToken.Value == L"else") {
        L.Scan();
        AST ElseBlock = CodeBlockNodeGenerator(L).Parse();
        if(ElseBlock.IsNotMatchNode()) {
            MakeException(L"IfStatementNodeGenerator : Expected a code block.");
        }
        return {AST::TreeType::IfElseStatement, {Condition, CodeBlock, ElseBlock}};
    }
    return {AST::TreeType::IfStatement, {Condition, CodeBlock}};
}
