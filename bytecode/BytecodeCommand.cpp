//
// Created by Jerry Chou on 2022/3/27.
//

#include "BytecodeCommand.hpp"

BytecodeCommand::BytecodeCommand() : Operand(), Command() {}

BytecodeCommand::BytecodeCommand(BytecodeCommand::Instruction Command, BytecodeOperandType Operand) : Command(Command),
                                                                                                      Operand(Operand) {}

XString BytecodeCommand::ToString() const {
    XString Result{};
    Result += InstructionKindToString(Command);
    Result += L" ";

    if (Command == Instruction::push_integer)
        Result += std::to_wstring(Operand.Integer);
    else if (Command == Instruction::push_decimal)
        Result += std::to_wstring(Operand.Decimal);
    else if (Command == Instruction::push_boolean)
        Result += Operand.Boolean ? L"true" : L"false";
    else if (Command == Instruction::add_integer or Command == Instruction::mod_integer or
             Command == Instruction::div_integer or Command == Instruction::sub_integer or
             Command == Instruction::mul_integer or Command == Instruction::negate_integer or
             Command == Instruction::int_to_bool or Command == Instruction::int_to_deci or
             Command == Instruction::add_decimal or Command == Instruction::mod_decimal or
             Command == Instruction::div_decimal or Command == Instruction::sub_decimal or
             Command == Instruction::mul_decimal or Command == Instruction::negate_decimal or
             Command == Instruction::deci_to_bool or Command == Instruction::deci_to_int or
             Command == Instruction::negate_boolean or Command == Instruction::bool_to_int or
             Command == Instruction::bool_to_deci);
    else
        Result += std::to_wstring(Operand.Reference);

    return Result;
}

XString BytecodeCommand::InstructionKindToString(BytecodeCommand::Instruction Inst) {
    switch (Inst) {
        case Instruction::push_integer:
            return L"push_integer";
        case Instruction::push_decimal:
            return L"push_decimal";
        case Instruction::push_boolean:
            return L"push_boolean";
        case Instruction::duplicate:
            return L"duplicate";
        case Instruction::add_integer:
            return L"add_integer";
        case Instruction::sub_integer:
            return L"sub_integer";
        case Instruction::mul_integer:
            return L"mul_integer";
        case Instruction::div_integer:
            return L"div_integer";
        case Instruction::mod_integer:
            return L"mod_integer";
        case Instruction::negate_integer:
            return L"negate_integer";
        case Instruction::add_decimal:
            return L"add_decimal";
        case Instruction::sub_decimal:
            return L"sub_decimal";
        case Instruction::mul_decimal:
            return L"mul_decimal";
        case Instruction::div_decimal:
            return L"div_decimal";
        case Instruction::mod_decimal:
            return L"mod_decimal";
        case Instruction::negate_decimal:
            return L"negate_decimal";
        case Instruction::negate_boolean:
            return L"negate_boolean";
        case Instruction::binary_or:
            return L"binary_or";
        case Instruction::binary_and:
            return L"binary_and";
        case Instruction::binary_xor:
            return L"binary_xor";
        case Instruction::binary_lm:
            return L"binary_lm";
        case Instruction::binary_rm:
            return L"binary_rm";
        case Instruction::logic_and:
            return L"logic_and";
        case Instruction::logic_or:
            return L"logic_or";
        case Instruction::logic_mt:
            return L"logic_mt";
        case Instruction::logic_lt:
            return L"logic_lt";
        case Instruction::logic_me:
            return L"logic_me";
        case Instruction::logic_le:
            return L"logic_le";
        case Instruction::increment_integer:
            return L"increment_integer";
        case Instruction::decrement_integer:
            return L"decrement_integer";
        case Instruction::increment_decimal:
            return L"increment_decimal";
        case Instruction::decrement_decimal:
            return L"decrement_decimal";
        case Instruction::store:
            return L"store";
        case Instruction::int_to_deci:
            return L"int_to_deci";
        case Instruction::deci_to_int:
            return L"deci_to_int";
        case Instruction::int_to_bool:
            return L"int_to_bool";
        case Instruction::bool_to_int:
            return L"bool_to_int";
        case Instruction::deci_to_bool:
            return L"deci_to_bool";
        case Instruction::bool_to_deci:
            return L"bool_to_deci";
        case Instruction::jump_if_true:
            return L"jump_if_true";
        case Instruction::jump_if_false:
            return L"jump_if_false";
        case Instruction::jump:
            return L"jump";
        case Instruction::new_array_int:
            return L"new_array_int";
        case Instruction::new_array_decimal:
            return L"new_array_decimal";
        case Instruction::new_array_boolean:
            return L"new_array_boolean";
        case Instruction::new_array_string:
            return L"new_array_string";
        case Instruction::new_array_class:
            return L"new_array_class";
        case Instruction::new_multi_dimension_array_int:
            return L"new_multi_dimension_array_int";
        case Instruction::new_multi_dimension_array_decimal:
            return L"new_multi_dimension_array_decimal";
        case Instruction::new_multi_dimension_array_boolean:
            return L"new_multi_dimension_array_boolean";
        case Instruction::new_multi_dimension_array_string:
            return L"new_multi_dimension_array_string";
        case Instruction::new_multi_dimension_array_class:
            return L"new_multi_dimension_array_class";
        case Instruction::array_index:
            return L"array_index";
        case Instruction::put_field:
            return L"put_field";
        case Instruction::get_field:
            return L"get_field";
        case Instruction::get_const_string:
            return L"get_const_string";
        case Instruction::push_function:
            return L"push_function";
        case Instruction::invoke_function:
            return L"invoke_function";
    }
    return {};
}
