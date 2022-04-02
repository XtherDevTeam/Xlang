//
// Created by Jerry Chou on 2022/4/2.
//

#include "BytecodeGenerator.hpp"

BytecodeGenerator::BytecodeGenerator() = default;

BytecodeCommandArray BytecodeGenerator::Generate(AST &Target) {
    BytecodeCommandArray Result;
    switch (Target.Type) {
        case AST::TreeType::Primary: {
            switch (Target.Node.Kind) {
                case Lexer::TokenKind::Integer: {
                    Result.PushCommand({BytecodeCommand::Instruction::push_integer, (BytecodeOperandType) {
                            std::stol(Target.Node.Value)}});
                    break;
                }
                case Lexer::TokenKind::Decimal: {
                    Result.PushCommand({BytecodeCommand::Instruction::push_decimal, (BytecodeOperandType) {
                            std::stof(Target.Node.Value)}});
                    break;
                }
                default: {
                    throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                    L"Unexpected primary token type.");
                }
            }
        }
        default: {
            Lexer::Token O = Target.GetFirstNotNullToken();
            throw BytecodeGenerateException(O.Line, O.Column,
                                            L"Unexpected primary token type.");
        }
    }
    return Result;
}
