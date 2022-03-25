//
// Created by Jerry Chou on 2022/3/25.
//

#include "XlangException.hpp"

#include <utility>

XlangException::XlangException(const XString& String) : String(wstring2string(String)) {}

const char * XlangException::what() const noexcept {
    return String.c_str();
}

XlangException::XlangException() = default;
