//
// Created by Jerry Chou on 2022/3/27.
//

#include "Typename.hpp"

#include <utility>

Typename::Typename() : Kind(), ClassIndexInGlobalEnvironment(0),
                       FunctionIndexInGlobalEnvironment(0) {

}

Typename::Typename(Typename::TypenameKind Kind) : Kind(Kind), ClassIndexInGlobalEnvironment(0),
                                                  FunctionIndexInGlobalEnvironment(0) {

}

Typename::Typename(Typename::TypenameKind Kind, XIndexType Index) : Kind(Kind) {
    switch (Kind) {
        case TypenameKind::Function: {
            FunctionIndexInGlobalEnvironment = Index;
            break;
        }
        case TypenameKind::Class: {
            ClassIndexInGlobalEnvironment = Index;
            break;
        }
        default: {
            break;
        }
    }
}

Typename &Typename::operator=(const Typename &rhs) = default;

Typename::~Typename() = default;
