//
// Created by Jerry Chou on 2022/3/26.
//

#include "MultiplicationExpressionNodeGenerator.hpp"

MultiplicationExpressionNodeGenerator::MultiplicationExpressionNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST MultiplicationExpressionNodeGenerator::Parse() {
    AST LeftTree = IncrementExpressionNodeGenerator(L).Parse();
    if (LeftTree.IsNotMatchNode()) {
        if ((LeftTree = DecrementExpressionNodeGenerator(L).Parse()).IsNotMatchNode()) {
            Rollback();
            return {};
        }
    }
    while (L.LastToken.Kind == Lexer::TokenKind::Asterisk or L.LastToken.Kind == Lexer::TokenKind::Slash or
           L.LastToken.Kind == Lexer::TokenKind::PercentSign) {
        AST Operator = {AST::TreeType::Operator, L.LastToken};
        L.Scan();
        AST RightTree = IndexExpressionNodeGenerator(L).Parse();
        if (RightTree.IsNotMatchNode()) {
            if ((LeftTree = DecrementExpressionNodeGenerator(L).Parse()).IsNotMatchNode()) {
                Rollback();
                return {};
            }
            MakeException(L"AdditionExpression: Expected a MultiplicationExpression");
        }
        LeftTree = {AST::TreeType::AdditionExpression, {LeftTree, Operator, RightTree}};
    }
    return LeftTree;
}
