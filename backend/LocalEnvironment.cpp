//
// Created by Jerry Chou on 2022/3/27.
//

#include "LocalEnvironment.hpp"

#include <utility>

LocalEnvironment::LocalEnvironment() : CreateBy(), ParentEnvironment(0), SymbolTable() {

}

LocalEnvironment::LocalEnvironment(XString CreateBy, XIndexType ParentEnvironment) : CreateBy(std::move(CreateBy)),
                                                                                     ParentEnvironment(
                                                                                             ParentEnvironment),
                                                                                     SymbolTable() {

}
