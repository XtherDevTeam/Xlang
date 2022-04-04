//
// Created by Jerry Chou on 2022/3/27.
//

#ifndef XLANG2_BYTECODECOMMANDARRAY_HPP
#define XLANG2_BYTECODECOMMANDARRAY_HPP

#include "../share/config.hpp"

#include "BytecodeCommand.hpp"

class BytecodeCommandArray {
public:
    XArray<BytecodeCommand> Set;

    /**
     * @biref Push a command to Set
     * @param Command Command to push
     * @return Reference to this structure
     */
    BytecodeCommandArray &PushCommand(BytecodeCommand Command);

    /**
     * @biref Merge a BytecodeCommandArray to the back of this Set
     * @param rhs Array to merge
     * @return Reference to this structure
     */
    BytecodeCommandArray &Merge(const BytecodeCommandArray &rhs);

    /**
     * @biref Return a string of this command array
     * @return A string
     */
    [[nodiscard]] XString ToString() const;
};


#endif //XLANG2_BYTECODECOMMANDARRAY_HPP
