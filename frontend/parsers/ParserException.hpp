//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_PARSEREXCEPTION_HPP
#define XLANG2_PARSEREXCEPTION_HPP

#include "../../share/XlangException.hpp"

class ParserException : XlangException {
    std::string message;
public:
    ParserException();

    ParserException(XInteger Line, XInteger Column, const XString &Reason);

    [[nodiscard]] const char *what() const noexcept override;
};


#endif //XLANG2_PARSEREXCEPTION_HPP
