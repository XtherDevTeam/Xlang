//
// Created by Jerry Chou on 2022/3/26.
//

#include "AssignmentExpressionNodeGenerator.hpp"

AssignmentExpressionNodeGenerator::AssignmentExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST AssignmentExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
