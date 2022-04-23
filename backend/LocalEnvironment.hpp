//
// Created by Jerry Chou on 2022/3/27.
//

#ifndef XLANG2_LOCALENVIRONMENT_HPP
#define XLANG2_LOCALENVIRONMENT_HPP

#include "../share/config.hpp"

#include "SymbolTableItem.hpp"

class LocalEnvironment {
public:
    XString CreateBy;
    XIndexType ParentEnvironment;
    XArray<SymbolTableItem> SymbolItem;
    XArray<XIndexType> InnerBlockFrames;

    LocalEnvironment();

    LocalEnvironment(XString CreateBy, XIndexType ParentEnvironment);

    void CreateInnerBlockFrame();

    void LeaveInnerBlockFrame();
};


#endif //XLANG2_LOCALENVIRONMENT_HPP
