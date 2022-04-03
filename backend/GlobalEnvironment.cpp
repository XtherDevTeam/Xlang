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

bool GlobalEnvironment::PushSymbolItem(XIndexType Environment, XString Key, const TypenameDerive &Value) {
    if (SearchSymbol(Environment, Key).second != -1) {
        return false;
    }
    Environments[Environment].SymbolItem.emplace_back(
            (SymbolTableItem) {std::move(Key), Value, SymbolTableItem::Access::Public, 0});
    Environments[Environment].SymbolItem.back().StackIndex = EmuStack.StackFrames.back().PushItem(
            (EmulateStack::Item) {Environment, Environments[Environment].SymbolItem.size() - 1});
    return true;
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

std::pair<XIndexType, XIndexType> GlobalEnvironment::SearchSymbol(XIndexType Environment, const XString &SymbolName) {
    for (XIndexType Index = Environment;
         Environments[Index].ParentEnvironment != Index; Index = Environments[Index].ParentEnvironment) {
        auto Iter = Environments[Index].SymbolItem.begin();
        for (; Iter != Environments[Index].SymbolItem.end(); Iter++) {
            if (Iter->Name == SymbolName) {
                return {Environment, Iter - Environments[Index].SymbolItem.begin()};
            }
        }
    }
    return {Environment, -1};
}

XIndexType GlobalEnvironment::PushConstantItem(const ConstantPoolItem &Item) {
    auto Iter = SearchConstantPoolItem(Item);
    if (Iter != ConstantPool.end())
        return Iter - ConstantPool.begin();

    ConstantPool.emplace_back(Item);
    /* return the index of the last element of constant pool */
    return ConstantPool.end()-- - ConstantPool.begin();
}

std::vector<ConstantPoolItem>::iterator GlobalEnvironment::SearchConstantPoolItem(const ConstantPoolItem &Item) {
    auto Iter = ConstantPool.begin();
    for (; Iter != ConstantPool.end(); Iter++) {
        if (*Iter == Item) {
            return Iter;
        }
    }
    return Iter;
}
