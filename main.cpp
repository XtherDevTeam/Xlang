#include <iostream>
#include "share/config.hpp"
#include "frontend/Lexer.hpp"

int main() {
    setlocale(LC_ALL, "zh_CN");
    Lexer Lex{L"print(114514, '\\'Hello,world!\\'\\n');"};
    try {
        for (Lexer::Token T = Lex.Scan(); T.Kind != Lexer::TokenKind::EoF; T = Lex.Scan()) {
            std::cout << wstring2string(T.Value) << "\n";
        }
    } catch (LexerException &E) {
        std::cout << E.what() << std::endl;
    }
    return 0;
}
