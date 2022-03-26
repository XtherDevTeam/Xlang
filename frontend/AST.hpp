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
public:
    enum class TreeType : int {
/* AST output AST kinds */
/* Primary */
        NotMatch,
        Primary,
        ExtendPrimary,
        Identifier,
        Arguments,
        Operator,
/* Expression */
        IndexExpression,
        FunctionCallingExpression,
        MemberExpression,
        NegativeExpression,
        IncrementExpression,
        DecrementExpression,
        MultiplicationExpression,
        AdditionExpression,
        BinaryMoveExpression,
        ComparingExpression,
        EqualExpression,
        BinaryExpression,
        LogicExpression,
/* rvalue Expression */
        AssignmentExpression
    } Type;
    Lexer::Token Node;
    XArray<AST> Subtrees;

    AST();

    AST(TreeType Type, Lexer::Token Node, XArray<AST> Subtrees);

    /**
     * @biref Construct a leaf node.
     * @param Type AST Kind of this node
     * @param Node Subtrees of this node
     */
    AST(TreeType Type, Lexer::Token Node);

    /**
     * @biref Construct a branch node.
     * @param Type AST Kind of this node
     * @param Subtrees Subtrees of this node
     */
    AST(TreeType Type, XArray<AST> Subtrees);

    /**
     * @biref Return this node is or isn't leaf node
     * @return A boolean value
     */
    XBoolean IsLeafNode() const;

    /**
     * Return this node is or isn't a failed to match node
     * @return A boolean value
     */
    XBoolean IsNotMatchNode() const;
};


#endif //XLANG2_AST_HPP
