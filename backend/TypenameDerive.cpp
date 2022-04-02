//
// Created by Jerry Chou on 2022/4/2.
//

#include "TypenameDerive.hpp"

TypenameDerive::TypenameDerive(XArray<XInteger> ArrayDimensions) : Kind(DeriveKind::ArrayDerive),
                                                                   AccessDerive(Access::Public),
                                                                   ArrayDimensions(std::move(ArrayDimensions)) {

}

TypenameDerive::TypenameDerive(XArray<Typename> FunctionArgumentsList) : Kind(DeriveKind::FunctionDerive),
                                                                         AccessDerive(Access::Public),
                                                                         ArrayDimensions(),
                                                                         FunctionArgumentsList(
                                                                                 std::move(FunctionArgumentsList)) {

}

TypenameDerive::TypenameDerive() : Kind(DeriveKind::NoDerive), AccessDerive(Access::Public), ArrayDimensions(),
                                   FunctionArgumentsList() {

}
