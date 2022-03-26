//
// Created by Jerry Chou on 2022/3/26.
//

#include "AdditionExpressionNodeGrammar.hpp"

AdditionExpressionNodeGrammar::AdditionExpressionNodeGrammar(Lexer &L) : BaseGenerator(L) {

}

AST AdditionExpressionNodeGrammar::Parse() {
    return BaseGenerator::Parse();
}
