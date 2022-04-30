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
    XString Tests[] = {
            L"{public Integer val;val += 114514;{public Integer val;val += 2;}}",
            L"{(114.514)=>Integer;}",
            L"{if(True){Integer A = 0;}}",
            L"{for (Integer I = 0;I <= 10;I++) {I++;}}"
            };
    Lexer Lex{Tests[3]};
    Lex.Scan();
    try {
        AST Result = CodeBlockNodeGenerator(Lex).Parse();
        std::cout << wstring2string(Generator.Generate(Result).ToString());
        std::cout << std::endl;
    } catch (BytecodeGenerateException &E) {
        std::cout << E.what() << std::endl;
    } catch (ParserException &E) {
        std::cout << E.what() << std::endl;
    }
    return 0;
}
