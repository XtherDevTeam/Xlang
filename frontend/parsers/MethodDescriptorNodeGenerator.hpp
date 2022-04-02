//
// Created by Jerry Chou on 2022/4/3.
//

#ifndef XLANG2_METHODDESCRIPTORNODEGENERATOR_HPP
#define XLANG2_METHODDESCRIPTORNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class MethodDescriptorNodeGenerator : BaseGenerator {
public:
    explicit MethodDescriptorNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_METHODDESCRIPTORNODEGENERATOR_HPP
