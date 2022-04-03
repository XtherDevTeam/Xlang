//
// Created by Jerry Chou on 2022/4/2.
//

#include "TypeSpecifierNodeGenerator.hpp"

TypeSpecifierNodeGenerator::TypeSpecifierNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST TypeSpecifierNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::Identifier) {
        AST First = {AST::TreeType::Identifier, L.LastToken};
        AST Second = {AST::TreeType::ArrayDimensionsCount, (Lexer::Token) {}};
        L.Scan();
        while (L.LastToken.Kind == Lexer::TokenKind::LeftBracket) {
            L.Scan();
            if (L.LastToken.Kind != Lexer::TokenKind::RightBracket) {
                MakeException(L"Expected a right bracket to close the left bracket.");
            }
            L.Scan();
            Second.TypeSpecifierArrayDimensionsCount++;
        }
        return {AST::TreeType::TypeSpecifier, {First, Second}};
    } else {
        return {};
    }
}
