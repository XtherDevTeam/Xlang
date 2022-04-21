#include <iostream>
#include "share/config.hpp"
#include "frontend/Lexer.hpp"
#include "frontend/parsers/ExpressionNodeGenerator.hpp"
#include "backend/BytecodeGenerator.hpp"
#include "frontend/parsers/StatementNodeGenerator.hpp"

int main() {
    setlocale(LC_ALL, "zh_CN");
    BytecodeGenerator Generator;
    Generator.Environment.EmuStack.CreateStackFrame(0);
    Lexer Lex{L"public Boolean val;"};
    Lex.Scan();
    try {
        AST Result = StatementNodeGenerator(Lex).Parse();
        std::cout << wstring2string(Generator.Generate(Result).ToString());
        std::cout << std::endl;
    } catch (BytecodeGenerateException &E) {
        std::cout << E.what() << std::endl;
    }
    return 0;
}
