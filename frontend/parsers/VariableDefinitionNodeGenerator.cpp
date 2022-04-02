//
// Created by Jerry Chou on 2022/4/3.
//

#include "VariableDefinitionNodeGenerator.hpp"

VariableDefinitionNodeGenerator::VariableDefinitionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST VariableDefinitionNodeGenerator::Parse() {
    AST Descriptors = {AST::TreeType::Descriptors, (XArray<AST>) {}};
    AST Temp = AccessDescriptorNodeGenerator(L).Parse();
    if (!Temp.IsNotMatchNode()) Descriptors.Subtrees.push_back(Temp);

    Temp = VariableDescriptorNodeGenerator(L).Parse();
    if (!Temp.IsNotMatchNode()) Descriptors.Subtrees.push_back(Temp);

    AST Typename = TypeSpecifierNodeGenerator(L).Parse();
    if (Typename.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    AST Identifier = IdentifierNodeGenerator(L).Parse();
    if (Identifier.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    if (L.LastToken.Kind != Lexer::TokenKind::AssignSign) {
        return {AST::TreeType::VariableDeclaration, {
                Descriptors, Typename, Identifier
        }};
    }
    L.Scan();
    AST Expression = ExpressionNodeGenerator(L).Parse();
    return {AST::TreeType::VariableDefinition, {
            Descriptors, Typename, Identifier, Expression
    }};
}
