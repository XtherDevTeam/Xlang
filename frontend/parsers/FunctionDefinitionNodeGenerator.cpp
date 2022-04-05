//
// Created by Jerry Chou on 2022/4/4.
//

#include "FunctionDefinitionNodeGenerator.hpp"


FunctionDefinitionNodeGenerator::FunctionDefinitionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST FunctionDefinitionNodeGenerator::Parse() {
    AST Descriptors = {AST::TreeType::Descriptors, (XArray<AST>) {}};
    AST Temp = AccessDescriptorNodeGenerator(L).Parse();
    if (!Temp.IsNotMatchNode()) Descriptors.Subtrees.push_back(Temp);

    Temp = MethodDescriptorNodeGenerator(L).Parse();
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

    AST Arguments = ArgumentsNodeGenerator(L).Parse();
    if (Arguments.IsNotMatchNode()) {
        Rollback();
        return {};
    }

    AST CodeBlock = CodeBlockNodeGenerator(L).Parse();
    if (CodeBlock.IsNotMatchNode()) {
        Rollback();
        return {};
    }

    return {AST::TreeType::FunctionDefinition, {Descriptors, Typename, Identifier, Arguments, CodeBlock}};
}
