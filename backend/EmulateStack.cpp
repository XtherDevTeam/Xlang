//
// Created by Jerry Chou on 2022/4/3.
//

#include "EmulateStack.hpp"

#include <utility>

EmulateStack::Item::Item() : Temp(), VariableIndex(0), VariableEnvironmentIndex(0) {

}

EmulateStack::Item::Item(TypenameDerive Temp) : Temp(std::move(Temp)) {

}

EmulateStack::Item::Item(XIndexType VariableEnvironmentIndex, XIndexType VariableIndex) : VariableEnvironmentIndex(
        VariableEnvironmentIndex), VariableIndex(VariableIndex) {

}

EmulateStack::Frame::Frame() : OwnerEnvironment(), Items() {

}

EmulateStack::Frame::Frame(XIndexType OwnerEnvironment) : OwnerEnvironment(OwnerEnvironment) {

}

XIndexType EmulateStack::Frame::PushItem(EmulateStack::Item ToPush) {
    Items.emplace_back(std::move(ToPush));
    return Items.size() - 1;
}

void EmulateStack::Frame::PopItem(XIndexType Count) {
    while (Count--)
        Items.pop_back();
}

EmulateStack::EmulateStack() : StackFrames() {

}

void EmulateStack::CreateStackFrame(XIndexType OwnerEnvironment) {
    StackFrames.push_back((Frame){OwnerEnvironment});
}