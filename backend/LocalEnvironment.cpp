//
// Created by Jerry Chou on 2022/3/27.
//

#include "LocalEnvironment.hpp"

#include <utility>

LocalEnvironment::LocalEnvironment() : CreateBy(), ParentEnvironment(0), SymbolItem() {

}

LocalEnvironment::LocalEnvironment(XString CreateBy, XIndexType ParentEnvironment) : CreateBy(std::move(CreateBy)),
                                                                                     ParentEnvironment(
                                                                                             ParentEnvironment),
                                                                                     SymbolItem() {

}

void LocalEnvironment::CreateInnerBlockFrame() {
    InnerBlockFrames.push_back(SymbolItem.size());
}

void LocalEnvironment::LeaveInnerBlockFrame() {
    XIndexType Top = InnerBlockFrames.back();
    while (SymbolItem.size() != Top) {
        SymbolItem.pop_back();
    }
    InnerBlockFrames.pop_back();
}
