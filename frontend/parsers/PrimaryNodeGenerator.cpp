//
// Created by Jerry Chou on 2022/3/25.
//

#include "PrimaryNodeGenerator.hpp"

PrimaryNodeGenerator::PrimaryNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST PrimaryNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::Integer or L.LastToken.Kind == Lexer::TokenKind::Decimal or L.LastToken.Kind == Lexer::TokenKind::String) {
        AST Result = {AST::TreeType::Primary, L.LastToken};
        L.Scan(); // prepare for next time.
        return Result;
    }
}
