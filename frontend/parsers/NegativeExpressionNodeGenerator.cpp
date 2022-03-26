//
// Created by Jerry Chou on 2022/3/26.
//

#include "NegativeExpressionNodeGenerator.hpp"

NegativeExpressionNodeGenerator::NegativeExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST NegativeExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
