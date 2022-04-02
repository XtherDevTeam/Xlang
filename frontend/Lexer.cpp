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
                ? TokenKind::ReservedWords : TokenKind::Identifier, TempStr, Line, Column};
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

            LastToken = {TokenKind::Decimal, TempStr, Line, Column};
        } else {
            LastToken = {TokenKind::Integer, TempStr, Line, Column};
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
        NextCharacter(); // Skip
        LastToken = {TokenKind::String, TempStr, Line, Column};
    }
        /* Expression symbols */
        /* Starts with + */
    else if (String[Position] == L'+') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::AdditionAssignment, L"+=", Line, Column};
        } else if (String[Position] == L'+') {
            NextCharacter();   // Read after '+'
            LastToken = {TokenKind::IncrementSign, L"++", Line, Column};
        } else {
            LastToken = {TokenKind::Plus, L"+", Line, Column};
        }
    }
        /* Starts with - */
    else if (String[Position] == L'-') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::SubtractionAssignment, L"-=", Line, Column};
        } else if (String[Position] == L'-') {
            NextCharacter();   // Read after '+'
            LastToken = {TokenKind::DecrementSign, L"--", Line, Column};
        } else {
            LastToken = {TokenKind::Minus, L"-", Line, Column};
        }
    }
        /* Starts with * */
    else if (String[Position] == L'*') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::MultiplicationAssignment, L"*=", Line, Column};
        } else {
            LastToken = {TokenKind::Asterisk, L"*", Line, Column};
        }
    }
        /* Starts with / */
    else if (String[Position] == L'/') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::DivisionAssignment, L"/=", Line, Column};
        } else {
            LastToken = {TokenKind::Slash, L"/", Line, Column};
        }
    }
        /* Starts with % */
    else if (String[Position] == L'%') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::RemainderAssignment, L"%=", Line, Column};
        } else {
            LastToken = {TokenKind::PercentSign, L"%", Line, Column};
        }
    }
        /* Binary symbols */
        /* Or */
    else if (String[Position] == L'|') {
        NextCharacter();
        if (String[Position] == L'|') {
            NextCharacter();
            LastToken = {TokenKind::LogicOr, L"||", Line, Column};
        } else {
            LastToken = {TokenKind::BinaryOr, L"|", Line, Column};
        }
    }
        /* And */
    else if (String[Position] == L'&') {
        NextCharacter();
        if (String[Position] == L'&') {
            NextCharacter();
            LastToken = {TokenKind::LogicAnd, L"&&", Line, Column};
        } else {
            LastToken = {TokenKind::BinaryAnd, L"&", Line, Column};
        }
    }
        /* XOR */
    else if (String[Position] == L'^') {
        NextCharacter();
        LastToken = {TokenKind::BinaryXOR, L"^", Line, Column};
    }
        /* Logic symbols */
        /* Starts with = */
    else if (String[Position] == L'=') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::Equal, L"==", Line, Column};
        } else {
            LastToken = {TokenKind::AssignSign, L"=", Line, Column};
        }
    }
        /* Starts with ! */
    else if (String[Position] == L'!') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::NotEqual, L"!=", Line, Column};
        }
        throw LexerException(Line, Column, L"Unknown token : " + std::to_wstring(String[Position]));
    }
        /* Starts with < */
    else if (String[Position] == L'<') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::LessThanOrEqual, L"<=", Line, Column};
        } else if (String[Position] == '<') {
            NextCharacter();   // Read after '>'
            LastToken = {TokenKind::BinaryLeftMove, L"<<", Line, Column};
        } else {
            LastToken = {TokenKind::LessThan, L"<", Line, Column};
        }
    }
        /* Starts with > */
    else if (String[Position] == L'>') {
        NextCharacter();
        if (String[Position] == L'=') {
            NextCharacter();   // Read after '='
            LastToken = {TokenKind::MoreThanOrEqual, L">=", Line, Column};
        } else if (String[Position] == '>') {
            NextCharacter();   // Read after '>'
            LastToken = {TokenKind::BinaryRightMove, L">>", Line, Column};
        } else {
            LastToken = {TokenKind::MoreThan, L">", Line, Column};
        }
    }
        /* Brackets symbols */
        /* Parentheses */
    else if (String[Position] == L'(') {
        NextCharacter();
        LastToken = {TokenKind::LeftParentheses, L"(", Line, Column};
    } else if (String[Position] == L')') {
        NextCharacter();
        LastToken = {TokenKind::RightParentheses, L")", Line, Column};
    }
        /* Brackets */
    else if (String[Position] == L'[') {
        NextCharacter();
        LastToken = {TokenKind::LeftBracket, L"[", Line, Column};
    } else if (String[Position] == L']') {
        NextCharacter();
        LastToken = {TokenKind::RightBracket, L"]", Line, Column};
    }
        /* Braces */
    else if (String[Position] == L'{') {
        NextCharacter();
        LastToken = {TokenKind::LeftBraces, L"{", Line, Column};
    } else if (String[Position] == L'}') {
        NextCharacter();
        LastToken = {TokenKind::RightBraces, L"}", Line, Column};
    }
        /* Split symbols */
        /* Semicolons */
    else if (String[Position] == L';') {
        NextCharacter();
        LastToken = {TokenKind::Semicolon, L";", Line, Column};
    }
        /* Colons */
    else if (String[Position] == L',') {
        NextCharacter();
        LastToken = {TokenKind::Colon, L",", Line, Column};
    }
        /* Dots */
    else if (String[Position] == L'.') {
        NextCharacter();
        LastToken = {TokenKind::Dot, L".", Line, Column};
    }
        /* End of file */
    else if (String[Position] == L'\0') {
        LastToken = {TokenKind::EoF, L"", Line, Column};
    }
        /* Unknown tokens */
    else {
        throw LexerException(Line, Column, L"Unknown token : " + std::to_wstring(String[Position]));
    }
    return LastToken;
}

void Lexer::Reset() {
    Line = Column = Position = 0;
}

Lexer::Token::Token() : Kind(TokenKind::EoF), Line(0), Column(0), Value() {}

Lexer::Token::Token(Lexer::TokenKind Kind, XString Value, XInteger Line, XInteger Column) : Kind(Kind), Line(Line),
                                                                                            Column(Column),
                                                                                            Value(std::move(Value)) {}

bool Lexer::Token::operator==(const Lexer::Token &rhs) const {
    return (Line == rhs.Line && Column == rhs.Column && Kind == rhs.Kind && Value == rhs.Value);
}

bool Lexer::Token::operator!=(const Lexer::Token &rhs) const {
    return (Line != rhs.Line && Column != rhs.Column && Kind == rhs.Kind && Value == rhs.Value);
}

LexerException::LexerException() = default;

LexerException::LexerException(XInteger Line, XInteger Column, const XString &Reason) {
    message = "[Position " + std::to_string(Line) + ":" + std::to_string(Column) + "] Lexer Error: " +
              wstring2string(Reason);
}

const char *LexerException::what() const noexcept {
    return message.c_str();
}
