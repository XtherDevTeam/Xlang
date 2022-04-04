//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_TYPENAMEDERIVE_HPP
#define XLANG2_TYPENAMEDERIVE_HPP

#include "../share/config.hpp"

#include "Typename.hpp"

class TypenameDerive : public Typename {
public:
    enum class DeriveKind : int {
        ArrayDerive,
        FunctionDerive,
        NoDerive
    } Kind;

    Typename OriginalType;

    /**
     * @biref Array dimensions for saving a array derive.
     */
    XIndexType ArrayDimensionCount;

    /**
     * @biref Arguments list for function derive.
     */
    XArray<TypenameDerive> FunctionArgumentsList;
    XFunctionIndexType FunctionIndexInPool{};

    /**
     * @biref Construct a array derive typename.
     * @param ArrayDimensions Array dimensions for saving a array derive.
     */
    explicit TypenameDerive(XIndexType ArrayDimensions);

    /**
     * @biref Construct a function derive typename.
     * @param FunctionArgumentsList Arguments list for function derive.
     */
    explicit TypenameDerive(XArray<TypenameDerive> FunctionArgumentsList, XFunctionIndexType FunctionIndex);

    /**
     * @biref Construct a no-derive typename with a original typename
     * @param OriginalTypename
     */
    explicit TypenameDerive(const Typename &OriginalTypename);

    /**
     * @biref Construct a no-derive typename.
     */
    TypenameDerive();

    bool operator==(const TypenameDerive &rhs) const;

    bool operator!=(const TypenameDerive &rhs) const;
};


#endif //XLANG2_TYPENAMEDERIVE_HPP
