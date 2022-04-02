//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_XLANGCLASS_HPP
#define XLANG2_XLANGCLASS_HPP

#include "../share/config.hpp"
#include "TypenameDerive.hpp"

class XlangClass {
public:
    XString ClassName;

    XClassIndexType Extends;

    XMap<XString, TypenameDerive> Members;

    XMap<XString, XClassIndexType> Subclass;

    XlangClass();

    XlangClass(XString ClassName);
};


#endif //XLANG2_XLANGCLASS_HPP
