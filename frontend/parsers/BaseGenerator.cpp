//
// Created by Jerry Chou on 2022/3/25.
//

#include "BaseGenerator.hpp"

AST BaseGenerator::Parse() {
    return {};
}

BaseGenerator::BaseGenerator(Lexer &L) : L(L), InitialLine(L.Line), InitialColumn(L.Column),
                                         InitialPosition(L.Position), InitialLastToken(L.LastToken) {}

void BaseGenerator::Rollback() {
    L.Line = InitialLine;
    L.Column = InitialColumn;
    L.Position = InitialPosition;
    L.LastToken = InitialLastToken;
}
