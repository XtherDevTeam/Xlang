//
// Created by Jerry Chou on 2022/3/21.
//

#ifndef XLANG2_CONFIG_HPP
#define XLANG2_CONFIG_HPP



#include <string>
#include <vector>
#include <map>

using XString = std::wstring;
using XInteger = long int;
using XDecimal = float;
using XBoolean = bool;
using XCharacter = wchar_t;
template <typename T>
using XArray = std::vector<T>;

#endif //XLANG2_CONFIG_HPP
