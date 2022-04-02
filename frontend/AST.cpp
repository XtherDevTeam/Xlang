//
// Created by Jerry Chou on 2022/3/25.
//

#include "AST.hpp"

#include <utility>

AST::AST() : Type(TreeType::NotMatch) {}

AST::AST(AST::TreeType Type, Lexer::Token Node, XArray<AST> Subtrees) : Type(Type), Node(std::move(Node)),
                                                                        Subtrees(std::move(Subtrees)) {}

AST::AST(AST::TreeType Type, Lexer::Token Node) : Type(Type), Subtrees(), Node(std::move(Node)) {}

AST::AST(AST::TreeType Type, XArray<AST> Subtrees) : Type(Type), Node(), Subtrees(std::move(Subtrees)) {}

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
            if (R != Lexer::Token())
                return R;
        }
        return {};
    }
}
