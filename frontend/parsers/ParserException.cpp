//
// Created by Jerry Chou on 2022/3/26.
//

#include "ParserException.hpp"

ParserException::ParserException() = default;

ParserException::ParserException(XInteger Line, XInteger Column, const XString &Reason) {
    message = "[Position " + std::to_string(Line) + ":" + std::to_string(Column) + "] Parser Error: " +
              wstring2string(Reason);
}

const char *ParserException::what() const noexcept {
    return message.c_str();
}
