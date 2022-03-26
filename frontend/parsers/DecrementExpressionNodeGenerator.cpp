//
// Created by Jerry Chou on 2022/3/26.
//

#include "DecrementExpressionNodeGenerator.hpp"

DecrementExpressionNodeGenerator::DecrementExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST DecrementExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
