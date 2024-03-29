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
        Class,
    } Kind;

    XClassIndexType ClassIndexInGlobalEnvironment{};

    XFunctionIndexType FunctionIndexInGlobalEnvironment{};

    /**
     * @biref Default constructor
     */
    Typename();

    /**
     * @biref Initialize a basic type typename.
     * @param Kind
     */
    explicit Typename(TypenameKind Kind);

    Typename(TypenameKind Kind, XIndexType Index);

    Typename &operator=(const Typename &rhs);

    bool operator==(const Typename &rhs) const;

    ~Typename();
};


#endif //XLANG2_TYPENAME_HPP
