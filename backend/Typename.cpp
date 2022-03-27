//
// Created by Jerry Chou on 2022/3/27.
//

#include "Typename.hpp"

#include <utility>

Typename::Typename() : Kind(TypenameKind::Integer), ArraySize(0), StringReferenceToConstantPool(0), Parents(), VTable(),
                       ArrayTypename(nullptr) {}

Typename::Typename(Typename::TypenameKind Kind) : Kind(Kind), ArraySize(0), StringReferenceToConstantPool(0),
                                                  ArrayTypename(nullptr) {}

Typename::Typename(XMap<XIndexType, Typename> Parents,
                   XMap<XIndexType, Typename> VTable) : Kind(TypenameKind::Class), Parents(std::move(Parents)),
                                                        VTable(std::move(VTable)), ArrayTypename(nullptr), ArraySize(0),
                                                        StringReferenceToConstantPool(0) {

}

Typename::Typename(XIndexType ArraySize, const Typename &ArrayTypename) : Kind(TypenameKind::Array),
                                                                          StringReferenceToConstantPool(0),
                                                                          ArraySize(ArraySize) {
    this->ArrayTypename = new Typename(ArrayTypename);
}

Typename::~Typename() {
    // I followed the message of CLion, if there are any overflow error, don't call me.
    delete this->ArrayTypename;
}

Typename::Typename(const Typename &rhs) : Kind(rhs.Kind), Parents(rhs.Parents), VTable(rhs.VTable),
                                          StringReferenceToConstantPool(rhs.StringReferenceToConstantPool),
                                          ArraySize(rhs.ArraySize) {
    if (rhs.ArrayTypename) {
        this->ArrayTypename = new Typename(*rhs.ArrayTypename);
    }
}
