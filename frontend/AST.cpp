//
// Created by Jerry Chou on 2022/3/25.
//

#include "AST.hpp"

#include <utility>

AST::AST() : Type() {}

AST::AST(AST::TreeType Type, Lexer::Token Node, XArray<AST> Subtrees) : Type(Type), Node(std::move(Node)),
                                                                        Subtrees(std::move(Subtrees)) {}

AST::AST(Lexer::Token Node) : Subtrees(), Node(std::move(Node)) {}

AST::AST(XArray<AST> Subtrees) : Node(), Subtrees(std::move(Subtrees)) {}

XBoolean AST::IsLeafNode() {
    return Subtrees.empty();
}
