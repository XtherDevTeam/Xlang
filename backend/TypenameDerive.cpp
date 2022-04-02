//
// Created by Jerry Chou on 2022/4/2.
//

#include "TypenameDerive.hpp"

TypenameDerive::TypenameDerive(XArray<XInteger> ArrayDimensions) : Kind(DeriveKind::ArrayDerive),
                                                                   ArrayDimensions(std::move(ArrayDimensions)) {

}

TypenameDerive::TypenameDerive(XArray<Typename> FunctionArgumentsList) : Kind(DeriveKind::FunctionDerive),
                                                                         ArrayDimensions(),
                                                                         FunctionArgumentsList(
                                                                                 std::move(FunctionArgumentsList)) {

}

TypenameDerive::TypenameDerive() : Kind(DeriveKind::NoDerive), ArrayDimensions(), FunctionArgumentsList() {

}
