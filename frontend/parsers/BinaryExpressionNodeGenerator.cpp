//
// Created by Jerry Chou on 2022/3/26.
//

#include "BinaryExpressionNodeGenerator.hpp"

BinaryExpressionNodeGenerator::BinaryExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST BinaryExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
