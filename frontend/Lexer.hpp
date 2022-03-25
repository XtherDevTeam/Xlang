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

static XArray<XString> LexerReservedWords{
        L"if", L"else", L"switch", L"for", L"while", L"var", L"const"
};

class LexerException : XlangException {
    std::string message;
public:
    LexerException();

    LexerException(XInteger Line, XInteger Column, const XString &Reason);

    [[nodiscard]] const char *what() const noexcept override;
};

class Lexer {
private:
    XString String;
public:
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
        EoF,
    };

    class Token {
    public:
        TokenKind Kind;
        XString Value;

        Token();

        Token(TokenKind Kind, XString Value);
    };

    Token LastToken;

    Lexer();

    explicit Lexer(XString String);

    XInteger Line{}, Column{}, Position{};

    XCharacter NextCharacter();

    Token Scan();
};


#endif //XLANG2_LEXER_HPP
