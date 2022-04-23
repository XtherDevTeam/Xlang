//
// Created by Jerry Chou on 2022/4/3.
//

#include "EmulateStack.hpp"

#include <utility>

EmulateStack::Item::Item() : Temp(), VariableIndex(0), VariableEnvironmentIndex(0) {

}

EmulateStack::Item::Item(TypenameDerive Temp) : Temp(std::move(Temp)), Kind(ItemKind::Temp) {

}

EmulateStack::Item::Item(XIndexType VariableEnvironmentIndex, XIndexType VariableIndex) : VariableEnvironmentIndex(
        VariableEnvironmentIndex), VariableIndex(VariableIndex), Kind(ItemKind::Variable) {

}

EmulateStack::Frame::Frame() : OwnerEnvironment(), Items() {

}

EmulateStack::Frame::Frame(XIndexType OwnerEnvironment) : OwnerEnvironment(OwnerEnvironment) {

}

XIndexType EmulateStack::Frame::PushItem(EmulateStack::Item ToPush) {
    Items.push_back(std::move(ToPush));
    return Items.size() - 1;
}

void EmulateStack::Frame::PopItem(XIndexType Count) {
    while (Count--)
        Items.pop_back();
}

void EmulateStack::Frame::CreateInnerBlockFrame() {
    InnerBlockFrames.push_back(Items.size());
}

void EmulateStack::Frame::LeaveInnerBlockFrame() {
    XIndexType Top = InnerBlockFrames.back();
    while (Items.size() != Top) {
        Items.pop_back();
    }
    InnerBlockFrames.pop_back();
}

EmulateStack::EmulateStack() : StackFrames() {
}

void EmulateStack::CreateStackFrame(XIndexType OwnerEnvironment) {
    StackFrames.push_back((Frame){OwnerEnvironment});
}
