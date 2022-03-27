//
// Created by Jerry Chou on 2022/3/27.
//

#include "BytecodeCommand.hpp"

BytecodeCommand::BytecodeCommand() : Operand(), Command() {}

BytecodeCommand::BytecodeCommand(BytecodeCommand::Instruction Command, BytecodeOperandType Operand) : Command(Command),
                                                                                                      Operand(Operand) {}
