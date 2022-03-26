//
// Created by Jerry Chou on 2022/3/26.
//

#include "IndexExpressionNodeGenerator.hpp"

IndexExpressionNodeGenerator::IndexExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST IndexExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
