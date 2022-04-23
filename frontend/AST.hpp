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
        Identifier,
        Arguments,
        Operator,
        TypeCastingExpression,
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
/* rvalue expression */
        AssignmentExpression,
/* literals */
        ArrayLiteral,
/* Descriptors */
        TypeSpecifier,
        AccessDescriptor,
        VariableDescriptor,
        MethodDescriptor,
        Descriptors,
        ArrayDimensionsCount,
/* Variable definition statements */
        VariableDefinition,
        VariableDeclaration,
/* Easy to implement statements */
        ReturnStatement,
        ContinueStatement,
        BreakStatement,
        CodeBlockStatement,
/* Function definition */
        FunctionDefinition
    } Type;
    Lexer::Token Node;
    XArray<AST> Subtrees;
    XIndexType TypeSpecifierArrayDimensionsCount;

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
    [[nodiscard]] XBoolean IsLeafNode() const;

    /**
     * @brief Return this node is or isn't a failed to match node
     * @return A boolean value
     */
    [[nodiscard]] XBoolean IsNotMatchNode() const;

    /**
     * @biref Get first non-null token
     * @return A token
     */
    Lexer::Token GetFirstNotNullToken();
};


#endif //XLANG2_AST_HPP
