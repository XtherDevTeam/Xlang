//
// Created by Jerry Chou on 2022/3/26.
//

#include "MultiplicationExpressionNodeGenerator.hpp"

MultiplicationExpressionNodeGenerator::MultiplicationExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST MultiplicationExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
