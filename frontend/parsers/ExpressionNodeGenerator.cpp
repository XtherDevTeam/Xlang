//
// Created by Jerry Chou on 2022/3/26.
//

#include "ExpressionNodeGenerator.hpp"

ExpressionNodeGenerator::ExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ExpressionNodeGenerator::Parse() {
    AST Result = AssignmentExpressionNodeGenerator(L).Parse();
    if (Result.IsNotMatchNode()) {
        Result = LogicExpressionNodeGenerator(L).Parse();
        return Result;
    }
    return Result;
}
