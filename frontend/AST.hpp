//
// Created by Jerry Chou on 2022/3/25.
//

#ifndef XLANG2_AST_HPP
#define XLANG2_AST_HPP

#include "../share/config.hpp"

#include "Lexer.hpp"

/**
 * @biref AST Tree structure.
 */
class AST {
    enum class TreeType : int {
#include "../share/G.txt"
    } Type;
    Lexer::Token Node;
    XArray<AST> Subtrees;

    AST();

    AST(TreeType Type, Lexer::Token Node, XArray<AST> Subtrees);

    /**
     * @biref Construct a leaf node.
     * @param Node Token of this node
     */
    AST(Lexer::Token Node);

    /**
     * @biref Construct a branch node.
     * @param Subtrees Subtrees of this node
     */
    AST(XArray<AST> Subtrees);

    /**
     * @biref Return this node is or isn't leaf node
     * @return A boolean value.
     */
    XBoolean IsLeafNode();
};


#endif //XLANG2_AST_HPP
