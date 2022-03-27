//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_BYTECODEGENERATEEXCEPTION_HPP
#define XLANG2_BYTECODEGENERATEEXCEPTION_HPP

#include "../share/XlangException.hpp"

class BytecodeGenerateException : XlangException {
    std::string message;
public:
    BytecodeGenerateException();

    BytecodeGenerateException(XInteger Line, XInteger Column, const XString &Reason);

    [[nodiscard]] const char *what() const noexcept override;
};


#endif //XLANG2_BYTECODEGENERATEEXCEPTION_HPP
