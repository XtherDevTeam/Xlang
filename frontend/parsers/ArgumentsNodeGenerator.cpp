//
// Created by Jerry Chou on 2022/3/26.
//

#include "ArgumentsNodeGenerator.hpp"

ArgumentsNodeGenerator::ArgumentsNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ArgumentsNodeGenerator::Parse() {
    if (L.LastToken.Kind != Lexer::TokenKind::LeftParentheses) {
        Rollback();
        return {};
    }
    L.Scan();
    AST Result{AST::TreeType::Arguments, (XArray<AST>){}};
    while(true) {
        AST Temp = ExpressionNodeGenerator(L).Parse();
        if (Temp.IsNotMatchNode()) {
            MakeException(L"Expected a rvalue expression.");
        }
        Result.Subtrees.emplace_back(Temp);
        if (L.LastToken.Kind != Lexer::TokenKind::Colon) break;
        L.Scan();
    }
    if (L.LastToken.Kind != Lexer::TokenKind::RightParentheses) {
        MakeException(L"Expected a right parentheses for close a arguments node.");
    }
    L.Scan();
    return Result;
}
