//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_GLOBALENVIRONMENT_HPP
#define XLANG2_GLOBALENVIRONMENT_HPP

#include "LocalEnvironment.hpp"

class GlobalEnvironment {
public:
    std::vector<LocalEnvironment> Environments;
    std::vector<XIndexType> RemovedList;

    void LeaveEnvironment(XIndexType LocalIndex);

    XIndexType CreateEnvironment(XString CreateBy, XIndexType Parent);

    void PushSymbolItem(XIndexType Environment, XString Key, const Typename& Value);
};


#endif //XLANG2_GLOBALENVIRONMENT_HPP
