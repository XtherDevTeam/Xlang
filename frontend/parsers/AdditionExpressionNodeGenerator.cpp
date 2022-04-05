//
// Created by Jerry Chou on 2022/3/26.
//

#include "AdditionExpressionNodeGenerator.hpp"

AdditionExpressionNodeGenerator::AdditionExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST AdditionExpressionNodeGenerator::Parse() {
    AST LeftTree = MultiplicationExpressionNodeGenerator(L).Parse();
    if (LeftTree.IsNotMatchNode()) {
        Rollback();
        return {};
    }
    while (L.LastToken.Kind == Lexer::TokenKind::Plus or L.LastToken.Kind == Lexer::TokenKind::Minus) {
        AST Operator = {AST::TreeType::Operator, L.LastToken};
        L.Scan();
        AST RightTree = MultiplicationExpressionNodeGenerator(L).Parse();
        if (RightTree.IsNotMatchNode()) {
            MakeException(L"AdditionExpression: Expected a MultiplicationExpression");
        }
        LeftTree = {AST::TreeType::AdditionExpression, {LeftTree, Operator, RightTree}};
    }
    return LeftTree;
}
