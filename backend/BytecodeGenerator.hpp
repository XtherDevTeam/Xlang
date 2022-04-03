//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_BYTECODEGENERATOR_HPP
#define XLANG2_BYTECODEGENERATOR_HPP

#include "../share/config.hpp"

#include "BytecodeGenerateException.hpp"

#include "../frontend/AST.hpp"
#include "GlobalEnvironment.hpp"

class BytecodeGenerator {
public:
    GlobalEnvironment Environment;
    XIndexType EnvIndex;

    BytecodeGenerator();

    BytecodeCommandArray Generate(AST &Target);

    TypenameDerive GetTypeOfAST(AST &Target);
};


#endif //XLANG2_BYTECODEGENERATOR_HPP
