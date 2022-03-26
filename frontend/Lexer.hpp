//
// Created by Jerry Chou on 2022/3/22.
//

#ifndef XLANG2_LEXER_HPP
#define XLANG2_LEXER_HPP

#include <algorithm>

#include "../share/config.hpp"
#include "../share/IOStream.hpp"
#include "../share/FileStream.hpp"
#include "../share/XlangException.hpp"

/**
 * Keywords in Xlang 2
 */
static XArray<XString> LexerReservedWords{
        L"if", L"else", L"switch", L"for", L"while", L"var", L"const"
};

/**
 * @biref Exceptions of Lexer in Xlang2
 */
class LexerException : XlangException {
    std::string message;
public:
    LexerException();

    LexerException(XInteger Line, XInteger Column, const XString &Reason);

    [[nodiscard]] const char *what() const noexcept override;
};

class Lexer {
private:
    /**
     * Source codes.
     */
    XString String;
public:
    /**
     * Kinds of tokens.
     */
    enum class TokenKind : int {
        Identifier,
        String,
        ReservedWords,
        Integer,
        Decimal,
        Plus,                       // +
        Minus,                      // -
        Asterisk,                   // *
        Slash,                      // /
        PercentSign,                // %
        BinaryXOR,                  // ^
        BinaryOr,                   // |
        BinaryAnd,                  // &
        IncrementSign,              // ++
        DecrementSign,              // --
        BinaryLeftMove,             // <<
        BinaryRightMove,            // >>
        AdditionAssignment,         // +=
        SubtractionAssignment,      // -=
        MultiplicationAssignment,   // *=
        DivisionAssignment,         // /=
        RemainderAssignment,        // %=
        MoreThan,                   // >
        LessThan,                   // <
        MoreThanOrEqual,            // >=
        LessThanOrEqual,            // <=
        Equal,                      // ==
        NotEqual,                   // !=
        LogicAnd,                   // &&
        LogicOr,                    // ||
        AssignSign,                 // =
        LeftParentheses,            // (
        RightParentheses,           // )
        LeftBracket,                // [
        RightBracket,               // ]
        LeftBraces,                 // {
        RightBraces,                // }
        Semicolon,                  // ;
        Colon,                      // ,
        Dot,                        // .
        EoF,
    };

    /**
     * Token type
     */
    class Token {
    public:
        TokenKind Kind;
        XString Value;

        Token();

        Token(TokenKind Kind, XString Value);
    };

    /**
     * The last token has been got
     */
    Token LastToken;

    /**
     * @biref Default constructor of Lexer
     */
    Lexer();

    /**
     * @biref Initialize a Lexer with source codes.
     * @param String Source codes
     */
    explicit Lexer(XString String);

    XInteger Line{}, Column{}, Position{};

    /**
     * @biref Get next character
     * @return Next character
     */
    XCharacter NextCharacter();

    /**
     * @biref Get next token
     * @return The next token
     */
    Token Scan();

    /**
     * @biref Reset Lexer to initial state.
     */
    void Reset();
};


#endif //XLANG2_LEXER_HPP
