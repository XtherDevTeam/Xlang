//
// Created by Jerry Chou on 2022/3/25.
//

#include "AST.hpp"

#include <utility>

AST::AST() : Type(TreeType::NotMatch), TypeSpecifierArrayDimensionsCount(0) {}

AST::AST(AST::TreeType Type, Lexer::Token Node, XArray<AST> Subtrees) : Type(Type), Node(std::move(Node)),
                                                                        Subtrees(std::move(Subtrees)),
                                                                        TypeSpecifierArrayDimensionsCount(0) {}

AST::AST(AST::TreeType Type, Lexer::Token Node) : Type(Type), Subtrees(), Node(std::move(Node)),
                                                  TypeSpecifierArrayDimensionsCount(0) {}

AST::AST(AST::TreeType Type, XArray<AST> Subtrees) : Type(Type), Node(), Subtrees(std::move(Subtrees)),
                                                     TypeSpecifierArrayDimensionsCount(0) {}

XBoolean AST::IsLeafNode() const {
    return Subtrees.empty();
}

XBoolean AST::IsNotMatchNode() const {
    return Type == TreeType::NotMatch;
}

Lexer::Token AST::GetFirstNotNullToken() {
    if (IsNotMatchNode()) {
        return {};
    } else if (IsLeafNode()) {
        return Node;
    } else {
        for (auto &i: Subtrees) {
            Lexer::Token R = i.GetFirstNotNullToken();
            if (R.Kind != Lexer::TokenKind::EoF)
                return R;
        }
        return {};
    }
}
