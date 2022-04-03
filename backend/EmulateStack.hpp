//
// Created by Jerry Chou on 2022/4/3.
//

#ifndef XLANG2_EMULATESTACK_HPP
#define XLANG2_EMULATESTACK_HPP

#include "../share/config.hpp"
#include "TypenameDerive.hpp"

class EmulateStack {
public:
    class Item {
    public:
        enum class ItemKind : int {
            Variable,
            Temp,
        } Kind;

        XIndexType VariableEnvironmentIndex;
        XIndexType VariableIndex;

        TypenameDerive Temp;

        Item();

        explicit Item(TypenameDerive Temp);

        Item(XIndexType VariableEnvironmentIndex, XIndexType VariableIndex);
    };

    class Frame {
    public:
        XIndexType OwnerEnvironment;
        XArray<Item> Items;

        Frame();

        explicit Frame(XIndexType OwnerEnvironment);

        XIndexType PushItem(Item ToPush);

        void PopItem(XIndexType Count);
    };

    std::vector<Frame> StackFrames;

    EmulateStack();

    void CreateStackFrame(XIndexType OwnerEnvironment);
};


#endif //XLANG2_EMULATESTACK_HPP
