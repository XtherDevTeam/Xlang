//
// Created by Jerry Chou on 2022/3/27.
//

#ifndef XLANG2_OPERAND_HPP
#define XLANG2_OPERAND_HPP

#include "../share/config.hpp"

union BytecodeOperandType {
    XInteger Integer;
    XDecimal Decimal;
    XBoolean Boolean;
    XIndexType Reference;

    BytecodeOperandType();

    explicit BytecodeOperandType(XInteger Integer);

    explicit BytecodeOperandType(XDecimal Decimal);

    explicit BytecodeOperandType(XBoolean Boolean);

    explicit BytecodeOperandType(XIndexType Reference);
};

#endif //XLANG2_OPERAND_HPP
