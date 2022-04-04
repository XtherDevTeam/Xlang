//
// Created by Jerry Chou on 2022/4/2.
//

#include "TypenameDerive.hpp"

TypenameDerive::TypenameDerive(XIndexType ArrayDimensions) : Kind(DeriveKind::ArrayDerive),
                                                             ArrayDimensionCount(ArrayDimensions) {

}

TypenameDerive::TypenameDerive(XArray<TypenameDerive> FunctionArgumentsList, XFunctionIndexType FunctionIndexInPool) :
        Kind(DeriveKind::FunctionDerive),
        ArrayDimensionCount(),
        FunctionArgumentsList(
                std::move(FunctionArgumentsList)),
        FunctionIndexInPool(FunctionIndexInPool) {

}

TypenameDerive::TypenameDerive() : Kind(DeriveKind::NoDerive), ArrayDimensionCount(),
                                   FunctionArgumentsList() {

}

TypenameDerive::TypenameDerive(const Typename &OriginalTypename) : Kind(DeriveKind::NoDerive),
                                                                   OriginalType(OriginalTypename),
                                                                   ArrayDimensionCount() {

}

bool TypenameDerive::operator==(const TypenameDerive &rhs) const {
    return Kind == rhs.Kind and OriginalType == rhs.OriginalType and
           FunctionArgumentsList == rhs.FunctionArgumentsList and ArrayDimensionCount == rhs.ArrayDimensionCount;
}

bool TypenameDerive::operator!=(const TypenameDerive &rhs) const {
    return !operator==(rhs);
}
