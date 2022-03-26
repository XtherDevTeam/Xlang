//
// Created by Jerry Chou on 2022/3/26.
//

#include "IdentifierNodeGenerator.hpp"

IdentifierNodeGenerator::IdentifierNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST IdentifierNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::Identifier) {
        AST Result {AST::TreeType::Identifier, L.LastToken};
        L.Scan();   // prepare for next-time's read.
        return Result;
    }
    Rollback( ); // rollback to initial state.
    return {};
}
