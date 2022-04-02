//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_XLANGFUNCTION_HPP
#define XLANG2_XLANGFUNCTION_HPP

#include "../share/config.hpp"

#include "Typename.hpp"
#include "../bytecode/BytecodeCommandArray.hpp"

class XlangFunction {
public:
    XString FunctionName;

    BytecodeCommandArray Bytecodes;
};


#endif //XLANG2_XLANGFUNCTION_HPP
