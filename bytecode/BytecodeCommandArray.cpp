//
// Created by Jerry Chou on 2022/3/27.
//

#include "BytecodeCommandArray.hpp"

BytecodeCommandArray &BytecodeCommandArray::PushCommand(BytecodeCommand Command) {
    Set.emplace_back(Command);
    return *this;
}

BytecodeCommandArray &BytecodeCommandArray::Merge(const BytecodeCommandArray &rhs) {
    for (auto &i: rhs.Set) {
        Set.emplace_back(i);
    }
    return *this;
}

XString BytecodeCommandArray::ToString() const {
    XString Result{};
    for (auto &i: Set) {
        Result += i.ToString() + L"\n";
    }
    return Result;
}
