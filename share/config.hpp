//
// Created by Jerry Chou on 2022/3/21.
//

#ifndef XLANG2_CONFIG_HPP
#define XLANG2_CONFIG_HPP



#include <string>
#include <vector>
#include <map>
#include <unordered_map>

using XString = std::wstring;
using XInteger = long int;
using XDecimal = float;
using XBoolean = bool;
using XIndexType = unsigned long long;
using XCharacter = wchar_t;
template <typename T>
using XArray = std::vector<T>;
template <typename T, typename T1>
using XMap = std::map<T, T1>;

#endif //XLANG2_CONFIG_HPP
