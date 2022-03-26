//
// Created by Jerry Chou on 2022/3/26.
//

#include "ArgumentsNodeGenerator.hpp"

ArgumentsNodeGenerator::ArgumentsNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ArgumentsNodeGenerator::Parse() {
    return BaseGenerator::Parse();
}
