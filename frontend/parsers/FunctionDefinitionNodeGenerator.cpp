//
// Created by Jerry Chou on 2022/4/4.
//

#include "FunctionDefinitionNodeGenerator.hpp"

FunctionDefinitionNodeGenerator::FunctionDefinitionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST FunctionDefinitionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
