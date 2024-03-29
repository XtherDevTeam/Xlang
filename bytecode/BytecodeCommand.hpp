//
// Created by Jerry Chou on 2022/3/27.
//

#ifndef XLANG2_BYTECODECOMMAND_HPP
#define XLANG2_BYTECODECOMMAND_HPP

#include "../share/config.hpp"
#include "Operand.hpp"

class BytecodeCommand {
public:
    enum class Instruction : int {
        /* push commands */
        push_integer,
        push_decimal,
        push_boolean,
        duplicate,
        pop_value,
        /* operating commands (integer) */
        add_integer,
        sub_integer,
        mul_integer,
        div_integer,
        mod_integer,
        negate_integer,
        /* operating commands (decimal) */
        add_decimal,
        sub_decimal,
        mul_decimal,
        div_decimal,
        mod_decimal,
        negate_decimal,
        /* operating commands (boolean) */
        negate_boolean,
        /* binary values operating commands */
        binary_or,
        binary_and,
        binary_xor,
        binary_lm,
        binary_rm,
        /* logic operating commands */
        logic_and,
        logic_or,
        logic_mt,
        logic_lt,
        logic_me,
        logic_le,
        /* assign operating commands */
        store,
        /* cast operating commands */
        int_to_deci,
        deci_to_int,
        int_to_bool,
        bool_to_int,
        deci_to_bool,
        bool_to_deci,
        /* flow controlling commands */
        jump_if_true,
        jump_if_false,
        jump,
        /* extended commands : array */
        new_array_int,
        new_array_decimal,
        new_array_boolean,
        new_array_string,
        new_array_class,
        new_multi_dimension_array_int,
        new_multi_dimension_array_decimal,
        new_multi_dimension_array_boolean,
        new_multi_dimension_array_string,
        new_multi_dimension_array_class,
        array_index,
        /* extended commands : class */
        put_field,
        get_field,
        up_cast,
        down_cast,
        /* extended commands : string */
        get_const_string,
        /* extended commands : functions */
        push_function,
        invoke_function,
        /* fake command in compiling times */
        fake_command_jump_out_of_loop,
        fake_command_skip_to_next_round_loop,
    } Command;

    static XString InstructionKindToString(Instruction Inst);

    BytecodeOperandType Operand;

    BytecodeCommand();

    BytecodeCommand(Instruction Command, BytecodeOperandType Operand);

    [[nodiscard]] XString ToString() const;
};


#endif //XLANG2_BYTECODECOMMAND_HPP
