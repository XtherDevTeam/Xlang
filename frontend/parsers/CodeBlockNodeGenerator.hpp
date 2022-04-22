//
// Created by Jerry Chou on 2022/4/4.
//

#ifndef XLANG2_CODEBLOCKNODEGENERATOR_HPP
#define XLANG2_CODEBLOCKNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "StatementNodeGenerator.hpp"

class CodeBlockNodeGenerator : BaseGenerator {
public:
    explicit CodeBlockNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_CODEBLOCKNODEGENERATOR_HPP
