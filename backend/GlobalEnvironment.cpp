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
    RemovedEnvironmentsList.emplace_back(LocalIndex);
}

XIndexType GlobalEnvironment::CreateEnvironment(XString CreateBy, XIndexType Parent) {
    if (RemovedEnvironmentsList.empty()) {
        Environments.emplace_back((LocalEnvironment) {std::move(CreateBy), Parent});
        return Environments.size() - 1;
    }
    XIndexType Key = RemovedEnvironmentsList.back();
    Environments[Key] = {CreateBy, Parent};
    return Key;
}

void GlobalEnvironment::PushSymbolItem(XIndexType Environment, XString Key, const TypenameDerive &Value) {
    Environments[Environment].SymbolTable.insert(std::make_pair(std::move(Key), Value));
}

void GlobalEnvironment::PushFunction(const XlangFunction &Function) {
    if (SearchFunction(Function.FunctionName) != FunctionPool.end()) {
        return;
    } else {
        FunctionPool.emplace_back(Function);
    }
}

std::vector<XlangFunction>::iterator GlobalEnvironment::SearchFunction(const XString &FunctionName) {
    for (auto Iter = FunctionPool.begin(); Iter != FunctionPool.end(); Iter++) {
        if (Iter->FunctionName == FunctionName)
            return Iter;
    }
    return FunctionPool.end();
}

std::vector<XlangClass>::iterator GlobalEnvironment::SearchClass(const XString &ClassName) {
    for (auto Iter = ClassPool.begin(); Iter != ClassPool.end(); Iter++) {
        if (Iter->ClassName == ClassName)
            return Iter;
    }
    return ClassPool.end();
}

void GlobalEnvironment::PushClass(const XlangClass &Class) {
    if (SearchClass(Class.ClassName) != ClassPool.end()) {
        return;
    } else {
        ClassPool.emplace_back(Class);
        return;
    }
}
