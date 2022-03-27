//
// Created by Jerry Chou on 2022/3/27.
//

#include "XlangClass.hpp"

XBoolean XlangClass::instanceOf(XIndexType Parent) const {
    return VTables.count(Parent);
}
