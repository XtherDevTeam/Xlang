//
// Created by Jerry Chou on 2022/3/27.
//

#ifndef XLANG2_LOCALENVIRONMENT_HPP
#define XLANG2_LOCALENVIRONMENT_HPP

#include "../share/config.hpp"

#include "Typename.hpp"

class LocalEnvironment {
public:
    XString CreateBy;
    XIndexType ParentEnvironment;
    XMap<XIndexType, Typename> SymbolTable;
};


#endif //XLANG2_LOCALENVIRONMENT_HPP
