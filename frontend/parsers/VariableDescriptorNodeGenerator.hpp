//
// Created by Jerry Chou on 2022/4/3.
//

#ifndef XLANG2_VARIABLEDESCRIPTORNODEGENERATOR_HPP
#define XLANG2_VARIABLEDESCRIPTORNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class VariableDescriptorNodeGenerator : BaseGenerator {
public:
    explicit VariableDescriptorNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_VARIABLEDESCRIPTORNODEGENERATOR_HPP
