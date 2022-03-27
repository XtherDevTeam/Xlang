//
// Created by Jerry Chou on 2022/3/27.
//

#ifndef XLANG2_TYPENAME_HPP
#define XLANG2_TYPENAME_HPP

#include "../share/config.hpp"

/**
 * @biref Information of typename during compile time.
 */
class Typename {
public:
    enum class TypenameKind : int {
        Integer,
        Decimal,
        Boolean,
        String,
        Array,
        Class,
    } Kind;

    /**
     * @biref Saving types of parent classes [TypenameHash -> ParentClass]
     */
    XMap<XIndexType, Typename> Parents;

    /**
     * @biref VTable of this class. [VariableNameHash -> TypeOfVariable]
     */
    XMap<XIndexType, Typename> VTable;

    XIndexType ArraySize;

    Typename *ArrayTypename;

    XIndexType StringReferenceToConstantPool;

    /**
     * @biref Default constructor
     */
    Typename();

    /**
     * @biref Initialize a basic type typename.
     * @param Kind
     */
    explicit Typename(TypenameKind Kind);

    /**
     * @biref Initialize a class typename
     * @param Parents Parents of this class
     * @param VTable VTable of this class
     */
    Typename(XMap<XIndexType, Typename> Parents, XMap<XIndexType, Typename> VTable);

    /**
     * @biref Initialize a array typename
     * @param ArraySize Size of an array
     * @param ArrayTypename Type of this array
     */
    Typename(XIndexType ArraySize, const Typename &ArrayTypename);

    Typename(const Typename &rhs);

    ~Typename();
};


#endif //XLANG2_TYPENAME_HPP
