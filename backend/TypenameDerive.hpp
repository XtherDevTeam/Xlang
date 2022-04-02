//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_TYPENAMEDERIVE_HPP
#define XLANG2_TYPENAMEDERIVE_HPP

#include "../share/config.hpp"

#include "Typename.hpp"

class TypenameDerive {
    enum class DeriveKind : int {
        ArrayDerive,
        FunctionDerive,
        NoDerive
    } Kind;
    Typename OriginalType;

    /**
     * @biref Array dimensions for saving a array derive.
     */
    XArray<XInteger> ArrayDimensions;

    /**
     * @biref Arguments list for function derive.
     */
    XArray<Typename> FunctionArgumentsList;

    /**
     * @biref Construct a array derive typename.
     * @param ArrayDimensions Array dimensions for saving a array derive.
     */
    explicit TypenameDerive(XArray<XInteger> ArrayDimensions);

    /**
     * @biref Construct a function derive typename.
     * @param FunctionArgumentsList Arguments list for function derive.
     */
    explicit TypenameDerive(XArray<Typename> FunctionArgumentsList);

    /**
     * @biref Construct a no-derive typename.
     */
    TypenameDerive();
};


#endif //XLANG2_TYPENAMEDERIVE_HPP
