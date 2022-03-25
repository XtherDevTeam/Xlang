//
// Created by Jerry Chou on 2022/3/22.
//

#include "Lexer.hpp"

#include <utility>

Lexer::Lexer() : Line(0), Column(0), Position(0) {}

Lexer::Lexer(XString String) : Line(0), Column(0), Position(0), String(std::move(String)) {
}

XCharacter Lexer::NextCharacter() {
    if (Position >= String.length()) {
        return L'\0';
    }
    XCharacter Result = String[Position];
    switch (Result) {
        case L'\n': {
            Line++;
            Column = 0;
            break;
        }
        default: {
            Column++;
            break;
        }
    }
    Position++;
    return Result;
}

Lexer::Token Lexer::Scan() {
    /* White spaces */
    while (String[Position] == L' ') NextCharacter();
    while (String[Position] == L'\t') NextCharacter();
    /* Literals */
    /* Identifiers or reserved words */
    if (IsAlpha(String[Position])) {
        XString TempStr;
        XInteger Start = Position;
        while (String[Position] and (IsAlpha(String[Position]) or IsDigit(String[Position]))) NextCharacter();
        TempStr = String.substr(Start, Position - Start);

        LastToken = {
                std::find(LexerReservedWords.begin(), LexerReservedWords.end(), TempStr) != LexerReservedWords.end()
                ? TokenKind::ReservedWords : TokenKind::Identifier, TempStr};
    }
        /* Digital literals */
    else if (IsDigit(String[Position])) {
        XString TempStr{};
        XInteger Start = Position;
        while (String[Position] and IsDigit(String[Position])) NextCharacter();
        TempStr = String.substr(Start, Position - Start);

        if (String[Position] == L'.') {
            Start = Position;
            while (String[Position] and IsDigit(String[Position])) NextCharacter();
            TempStr += L'.' + String.substr(Start, Position - Start);

            LastToken = {TokenKind::Decimal, TempStr};
        } else {
            LastToken = {TokenKind::Integer, TempStr};
        }
    }
        /* String literals */
    else if (String[Position] == L'\'' or String[Position] == L'"') {
        XString TempStr{};
        XCharacter Type = String[Position];
        NextCharacter();
        XInteger Start = Position;
        while (String[Position] and String[Position] != Type) {
            if (String[Position] == L'\\') {
                NextCharacter();
            }
            NextCharacter();
        }
        TempStr = String.substr(Start, Position - Start);

        LastToken = {TokenKind::String, TempStr};
    }
        /* Expression symbols */
        /* Starts with + */
    else if (String[Position] == L'+') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::AdditionAssignment, L"+="};
        } else {
            LastToken = {TokenKind::Plus, L"+"};
        }
    }
        /* Starts with - */
    else if (String[Position] == L'-') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::SubtractionAssignment, L"-="};
        } else {
            LastToken = {TokenKind::Minus, L"-"};
        }
    }
        /* Starts with * */
    else if (String[Position] == L'*') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::MultiplicationAssignment, L"*="};
        } else {
            LastToken = {TokenKind::Asterisk, L"*"};
        }
    }
        /* Starts with / */
    else if (String[Position] == L'/') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::DivisionAssignment, L"/="};
        } else {
            LastToken = {TokenKind::Slash, L"/"};
        }
    }
        /* Starts with % */
    else if (String[Position] == L'%') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::RemainderAssignment, L"%="};
        } else {
            LastToken = {TokenKind::PercentSign, L"%"};
        }
    }
        /* Binary symbols */
        /* Or */
    else if (String[Position] == L'|') {
        NextCharacter();
        if (String[Position] == L'|') {
            NextCharacter();
            LastToken = {TokenKind::LogicOr, L"||"};
        } else {
            LastToken = {TokenKind::BinaryOr, L"|"};
        }
    }
        /* And */
    else if (String[Position] == L'&') {
        NextCharacter();
        if (String[Position] == L'&') {
            NextCharacter();
            LastToken = {TokenKind::LogicAnd, L"&&"};
        } else {
            LastToken = {TokenKind::BinaryAnd, L"&"};
        }
    }
        /* XOR */
    else if (String[Position] == L'^') {
        NextCharacter();
        LastToken = {TokenKind::BinaryXOR, L"^"};
    }
        /* Logic symbols */
        /* Starts with = */
    else if (String[Position] == L'=') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::Equal, L"=="};
        } else {
            LastToken = {TokenKind::AssignSign, L"="};
        }
    }
        /* Starts with ! */
    else if (String[Position] == L'!') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::NotEqual, L"!="};
        }
        throw LexerException(Line, Column, L"Unknown token : " + std::to_wstring(String[Position]));
    }
        /* Starts with < */
    else if (String[Position] == L'<') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::LessThanOrEqual, L"<="};
        } else if (String[Position] == '<') {
            NextCharacter();   // Read after '>'
            LastToken = {TokenKind::BinaryLeftMove, L"<<"};
        } else {
            LastToken = {TokenKind::LessThan, L"<"};
        }
    }
        /* Starts with > */
    else if (String[Position] == L'>') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::MoreThanOrEqual, L">="};
        } else if (String[Position] == '>') {
            NextCharacter();   // Read after '>'
            LastToken = {TokenKind::BinaryRightMove, L">>"};
        } else {
            LastToken = {TokenKind::MoreThan, L">"};
        }
    }
        /* Brackets symbols */
        /* Parentheses */
    else if (String[Position] == L'(') {
        NextCharacter();
        LastToken = {TokenKind::LeftParentheses, L"("};
    } else if (String[Position] == L')') {
        NextCharacter();
        LastToken = {TokenKind::RightParentheses, L")"};
    }
        /* Brackets */
    else if (String[Position] == L'[') {
        NextCharacter();
        LastToken = {TokenKind::LeftBracket, L"["};
    } else if (String[Position] == L']') {
        NextCharacter();
        LastToken = {TokenKind::RightBracket, L"]"};
    }
        /* Braces */
    else if (String[Position] == L'{') {
        NextCharacter();
        LastToken = {TokenKind::LeftBraces, L"{"};
    } else if (String[Position] == L'}') {
        NextCharacter();
        LastToken = {TokenKind::RightBraces, L"}"};
    }
        /* Split symbols */
        /* Semicolons */
    else if (String[Position] == L';') {
        NextCharacter();
        LastToken = {TokenKind::Semicolon, L";"};
    }
        /* Colons */
    else if (String[Position] == L',') {
        NextCharacter();
        LastToken = {TokenKind::Colon, L","};
    }
        /* End of file */
    else if (String[Position] == L'\0') {
        LastToken = {TokenKind::EoF, L""};
    }
        /* Unknown tokens */
    else {
        throw LexerException(Line, Column, L"Unknown token : " + std::to_wstring(String[Position]));
    }
    return LastToken;
}

Lexer::Token::Token() : Kind(TokenKind::EoF), Value() {}

Lexer::Token::Token(Lexer::TokenKind Kind, XString Value) : Kind(Kind), Value(std::move(Value)) {}

LexerException::LexerException() = default;

LexerException::LexerException(XInteger Line, XInteger Column, const XString &Reason) {
    message = "[Position " + std::to_string(Line) + ":" + std::to_string(Column) + "] Lexer Error: " +
              wstring2string(Reason);
}

const char *LexerException::what() const noexcept {
    return message.c_str();
}
