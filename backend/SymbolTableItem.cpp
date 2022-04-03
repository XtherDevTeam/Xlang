//
// Created by Jerry Chou on 2022/4/3.
//

#include "SymbolTableItem.hpp"

#include <utility>

SymbolTableItem::SymbolTableItem() : Name(), Type(), AccessDescriptor(Access::Public) {

}

SymbolTableItem::SymbolTableItem(XString Name, TypenameDerive Type, SymbolTableItem::Access AccessDescriptor,
                                 XIndexType StackIndex) : Name(std::move(Name)), Type(std::move(Type)),
                                                          AccessDescriptor(AccessDescriptor), StackIndex(StackIndex) {

}
