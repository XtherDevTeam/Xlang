//
// Created by Jerry Chou on 2022/3/26.
//

#include "EqualExpressionNodeGenerator.hpp"

EqualExpressionNodeGenerator::EqualExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST EqualExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
