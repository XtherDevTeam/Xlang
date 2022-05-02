//
// Created by Jerry Chou on 2022/5/2.
//

#ifndef XLANG2_GLOBALSTATEMENTNODEGENERATOR_HPP
#define XLANG2_GLOBALSTATEMENTNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

#include "VariableDefinitionNodeGenerator.hpp"
#include "FunctionDefinitionNodeGenerator.hpp"

class GlobalStatementNodeGenerator : BaseGenerator {
public:
    explicit GlobalStatementNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_GLOBALSTATEMENTNODEGENERATOR_HPP
