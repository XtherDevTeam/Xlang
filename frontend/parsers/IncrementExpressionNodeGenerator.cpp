//
// Created by Jerry Chou on 2022/3/26.
//

#include "IncrementExpressionNodeGenerator.hpp"

IncrementExpressionNodeGenerator::IncrementExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST IncrementExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
