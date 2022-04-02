//
// Created by Jerry Chou on 2022/4/2.
//

#include "GlobalEnvironment.hpp"

#include <utility>

void GlobalEnvironment::LeaveEnvironment(XIndexType LocalIndex) {
    if (LocalIndex == Environments.size() - 1) {
        Environments.pop_back();
    }
    Environments[LocalIndex] = {};
    RemovedList.emplace_back(LocalIndex);
}

XIndexType GlobalEnvironment::CreateEnvironment(XString CreateBy, XIndexType Parent) {
    if (RemovedList.empty()) {
        Environments.emplace_back((LocalEnvironment) {std::move(CreateBy), Parent});
        return Environments.size() - 1;
    }
    XIndexType Key = RemovedList.back();
    Environments[Key] = {CreateBy, Parent};
    return Key;
}

void GlobalEnvironment::PushSymbolItem(XIndexType Environment, XString Key, const Typename &Value) {
    Environments[Environment].SymbolTable.insert(std::make_pair(std::move(Key), Value));
}
