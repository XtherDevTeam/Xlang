//
// Created by Jerry Chou on 2022/4/3.
//

#include "MethodDescriptorNodeGenerator.hpp"

MethodDescriptorNodeGenerator::MethodDescriptorNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST MethodDescriptorNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::ReservedWords) {
        if (L.LastToken.Value == L"override" or L.LastToken.Value == L"virtual" or L.LastToken.Value == L"final") {
            AST Result{AST::TreeType::MethodDescriptor, L.LastToken};
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
