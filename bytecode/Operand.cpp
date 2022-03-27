//
// Created by Jerry Chou on 2022/3/27.
//

#include "Operand.hpp"

BytecodeOperandType::BytecodeOperandType() : Integer(0) {

}

BytecodeOperandType::BytecodeOperandType(XInteger Integer) : Integer(Integer) {

}

BytecodeOperandType::BytecodeOperandType(XDecimal Decimal) : Decimal(Decimal) {

}

BytecodeOperandType::BytecodeOperandType(XBoolean Boolean) : Boolean(Boolean) {

}

BytecodeOperandType::BytecodeOperandType(XIndexType Reference) : Reference(Reference) {

}
