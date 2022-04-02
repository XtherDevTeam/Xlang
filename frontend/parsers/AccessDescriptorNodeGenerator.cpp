//
// Created by Jerry Chou on 2022/4/3.
//

#include "AccessDescriptorNodeGenerator.hpp"

AccessDescriptorNodeGenerator::AccessDescriptorNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST AccessDescriptorNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::ReservedWords) {
        if (L.LastToken.Value == L"public" or L.LastToken.Value == L"private") {
            AST Result{AST::TreeType::AccessDescriptor, L.LastToken};
            L.Scan();
            return Result;
        } else {
            Rollback();
            return {};
        }
    } else {
        Rollback();
        return {};
    }
}
