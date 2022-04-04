//
// Created by Jerry Chou on 2022/4/4.
//

#include "ContinueStatementNodeGenerator.hpp"

ContinueStatementNodeGenerator::ContinueStatementNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ContinueStatementNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::ReservedWords and L.LastToken.Value == L"continue") {
        L.Scan();
        return {AST::TreeType::ContinueStatement, (Lexer::Token) {}};
    }
    Rollback();
    return {};
}
