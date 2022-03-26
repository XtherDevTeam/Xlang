//
// Created by Jerry Chou on 2022/3/26.
//

#include "LogicExpressionNodeGenerator.hpp"

LogicExpressionNodeGenerator::LogicExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST LogicExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
