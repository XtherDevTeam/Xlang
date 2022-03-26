//
// Created by Jerry Chou on 2022/3/26.
//

#include "MemberExpressionNodeGenerator.hpp"

MemberExpressionNodeGenerator::MemberExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST MemberExpressionNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
