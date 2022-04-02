//
// Created by Jerry Chou on 2022/4/3.
//

#ifndef XLANG2_VARIABLEDEFINITIONNODEGENERATOR_HPP
#define XLANG2_VARIABLEDEFINITIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "TypeSpecifierNodeGenerator.hpp"
#include "IdentifierNodeGenerator.hpp"
#include "ExpressionNodeGenerator.hpp"
#include "VariableDescriptorNodeGenerator.hpp"
#include "AccessDescriptorNodeGenerator.hpp"

class VariableDefinitionNodeGenerator : BaseGenerator {
public:
    explicit VariableDefinitionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_VARIABLEDEFINITIONNODEGENERATOR_HPP
