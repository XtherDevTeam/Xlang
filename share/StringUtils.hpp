//
// Created by Jerry Chou on 2022/3/22.
//

#ifndef XLANG2_STRINGUTILS_HPP
#define XLANG2_STRINGUTILS_HPP

#include "config.hpp"

#include <cstdlib>

#include <cstring>

std::string wstring2string(const std::wstring& ws);

std::wstring string2wstring(const std::string& s);

bool IsDigit(wchar_t Char);

bool IsAlpha(wchar_t Char);


#endif //XLANG2_STRINGUTILS_HPP
