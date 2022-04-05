//
// Created by Jerry Chou on 2022/4/4.
//

#ifndef XLANG2_FUNCTIONDEFINITIONNODEGENERATOR_HPP
#define XLANG2_FUNCTIONDEFINITIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "AccessDescriptorNodeGenerator.hpp"
#include "MethodDescriptorNodeGenerator.hpp"
#include "TypeSpecifierNodeGenerator.hpp"
#include "IdentifierNodeGenerator.hpp"
#include "ArgumentsNodeGenerator.hpp"
#include "CodeBlockNodeGenerator.hpp"

class FunctionDefinitionNodeGenerator : BaseGenerator {
public:
    explicit FunctionDefinitionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_FUNCTIONDEFINITIONNODEGENERATOR_HPP
