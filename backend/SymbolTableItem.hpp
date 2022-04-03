//
// Created by Jerry Chou on 2022/4/3.
//

#ifndef XLANG2_SYMBOLTABLEITEM_HPP
#define XLANG2_SYMBOLTABLEITEM_HPP

#include "../share/config.hpp"
#include "TypenameDerive.hpp"

class SymbolTableItem {
public:
    XString Name;
    TypenameDerive Type;
    enum class Access : int {
        Public,
        Private,
    } AccessDescriptor;
    XIndexType StackIndex;

    SymbolTableItem();

    SymbolTableItem(XString Name, TypenameDerive Type, Access AccessDescriptor, XIndexType StackIndex);
};


#endif //XLANG2_SYMBOLTABLEITEM_HPP
