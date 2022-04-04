//
// Created by Jerry Chou on 2022/4/4.
//

#include "HashLib.hpp"

XIndexType HashLib::StringHash(XString String) {
    return std::hash<XString>()(String);
}
