//
// Created by Jerry Chou on 2022/3/27.
//

#ifndef XLANG2_BYTECODEGENERATOR_HPP
#define XLANG2_BYTECODEGENERATOR_HPP

#include "../share/config.hpp"
#include "Typename.hpp"
#include "LocalEnvironment.hpp"
#include "../frontend/AST.hpp"
#include "../bytecode/BytecodeCommandArray.hpp"

class BytecodeGenerator {
public:
    AST Source;
    XArray<LocalEnvironment> EnvironmentStack;

    BytecodeGenerator();

    explicit BytecodeGenerator(AST Source);

    BytecodeCommandArray Generate(AST &Target);
};


#endif //XLANG2_BYTECODEGENERATOR_HPP
