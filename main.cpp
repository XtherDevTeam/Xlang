#include <iostream>
#include "share/config.hpp"
#include "frontend/Lexer.hpp"
#include "frontend/parsers/ExpressionNodeGenerator.hpp"

int main() {
    setlocale(LC_ALL, "zh_CN");
    Lexer Lex{L"0 - -1"};
    try {
        for (Lexer::Token T = Lex.Scan(); T.Kind != Lexer::TokenKind::EoF; T = Lex.Scan()) {
            std::cout << wstring2string(T.Value) << "\n" << std::flush;
        }
    } catch (LexerException &E) {
        std::cout << E.what() << std::endl;
    }
    Lex.Reset();
    Lex.Scan();
    try {
        AST Tree = ExpressionNodeGenerator(Lex).Parse();
        std::cout << "?" << std::endl;
    } catch (ParserException &E) {
        std::cout << E.what() << std::endl;
    }
    return 0;
}
