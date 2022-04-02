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
    XMap<XString, Typename> SymbolTable;

    LocalEnvironment();

    LocalEnvironment(XString CreateBy, XIndexType ParentEnvironment);
};


#endif //XLANG2_LOCALENVIRONMENT_HPP
