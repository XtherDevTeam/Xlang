//
// Created by Jerry Chou on 2022/4/2.
//

#include "TypenameDerive.hpp"

TypenameDerive::TypenameDerive(XIndexType ArrayDimensions) : Kind(DeriveKind::ArrayDerive),
                                                             ArrayDimensionCount(ArrayDimensions) {

}

TypenameDerive::TypenameDerive(XArray<Typename> FunctionArgumentsList) : Kind(DeriveKind::FunctionDerive),
                                                                         ArrayDimensionCount(),
                                                                         FunctionArgumentsList(
                                                                                 std::move(FunctionArgumentsList)) {

}

TypenameDerive::TypenameDerive() : Kind(DeriveKind::NoDerive), ArrayDimensionCount(),
                                   FunctionArgumentsList() {

}

TypenameDerive::TypenameDerive(const Typename &OriginalTypename) : Kind(DeriveKind::NoDerive),
                                                                   OriginalType(OriginalTypename),
                                                                   ArrayDimensionCount() {

}
