//
// Created by Jerry Chou on 2022/3/26.
//

#include "BinaryMoveExpressionNodeGenerator.hpp"

BinaryMoveExpressionNodeGenerator::BinaryMoveExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST BinaryMoveExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
