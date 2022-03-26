//
// Created by Jerry Chou on 2022/3/26.
//

#include "ComparingExpressionNodeGenerator.hpp"

ComparingExpressionNodeGenerator::ComparingExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ComparingExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
