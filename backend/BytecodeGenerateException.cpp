//
// Created by Jerry Chou on 2022/3/26.
//

#include "BytecodeGenerateException.hpp"

BytecodeGenerateException::BytecodeGenerateException() = default;

BytecodeGenerateException::BytecodeGenerateException(XInteger Line, XInteger Column, const XString &Reason) {
    message = "[Position " + std::to_string(Line) + ":" + std::to_string(Column) + "] Bytecode Generate Error: " +
              wstring2string(Reason);
}

const char *BytecodeGenerateException::what() const noexcept {
    return message.c_str();
}
