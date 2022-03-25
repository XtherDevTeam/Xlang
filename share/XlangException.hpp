//
// Created by Jerry Chou on 2022/3/25.
//

#ifndef XLANG2_XLANGEXCEPTION_HPP
#define XLANG2_XLANGEXCEPTION_HPP

#include <exception>
#include "config.hpp"
#include "StringUtils.hpp"

class XlangException : std::exception {
    std::string String;
public:
    XlangException();

    explicit XlangException(const XString&  String);

    [[nodiscard]] const char * what() const noexcept override;
};


#endif //XLANG2_XLANGEXCEPTION_HPP
