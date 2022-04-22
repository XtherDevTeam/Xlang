//
// Created by Jerry Chou on 2022/4/4.
//

#include "CodeBlockNodeGenerator.hpp"

CodeBlockNodeGenerator::CodeBlockNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST CodeBlockNodeGenerator::Parse() {
    if (L.LastToken.Kind != Lexer::TokenKind::LeftBraces) {
        return {};
    }
    L.Scan();
    XArray<AST> StatementList;
    for (auto Stmt = StatementNodeGenerator(L).Parse(); !Stmt.IsNotMatchNode();) {
        if (L.LastToken.Kind == Lexer::TokenKind::Semicolon) {
            L.Scan();
        } else {
            MakeException(L"CodeBlockNodeGenerator: Excepted a semicolon after statement.");
        }
        StatementList.emplace_back(Stmt);
        Stmt = StatementNodeGenerator(L).Parse();
    }
    if (L.LastToken.Kind != Lexer::TokenKind::RightBraces) {
        MakeException(L"CodeBlockNodeGenerator: Excepted a right brace to close the code block.");
    }
    AST Result {AST::TreeType::CodeBlockStatement, StatementList};
    return Result;
}
