//
// Created by Jerry Chou on 2022/4/4.
//

#include "BreakStatementNodeGenerator.hpp"

BreakStatementNodeGenerator::BreakStatementNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST BreakStatementNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::ReservedWords and L.LastToken.Value == L"break") {
        L.Scan();
        return {AST::TreeType::BreakStatement, (Lexer::Token) {}};
    }
    Rollback();
    return {};
}
