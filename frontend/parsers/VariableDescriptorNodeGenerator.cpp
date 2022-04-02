//
// Created by Jerry Chou on 2022/4/3.
//

#include "VariableDescriptorNodeGenerator.hpp"

VariableDescriptorNodeGenerator::VariableDescriptorNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST VariableDescriptorNodeGenerator::Parse() {
    if (L.LastToken.Kind == Lexer::TokenKind::ReservedWords) {
        if (L.LastToken.Value == L"const" or L.LastToken.Value == L"final") {
            AST Result{AST::TreeType::VariableDescriptor, L.LastToken};
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
