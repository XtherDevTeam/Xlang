//
// Created by Jerry Chou on 2022/4/2.
//

#include "TypeSpecifierNodeGenerator.hpp"

TypeSpecifierNodeGenerator::TypeSpecifierNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST TypeSpecifierNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::Identifier) {
        AST First = {AST::TreeType::Identifier, L.LastToken};
        L.Scan();
        if (L.LastToken.Kind != Lexer::TokenKind::LeftBracket) {
            return {AST::TreeType::TypeSpecifier, {First}};
        }
        L.Scan();
        if (L.LastToken.Kind != Lexer::TokenKind::RightBracket) {
            Rollback();
            return {};
        }

        AST Second = {AST::TreeType::ArrayLiteral, (XArray<AST>) {}};
        L.Scan();

        return {AST::TreeType::TypeSpecifier, {First, Second}};
    } else {
        return {};
    }
}
