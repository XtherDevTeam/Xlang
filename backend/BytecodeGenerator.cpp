#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by Jerry Chou on 2022/4/2.
//

#include "BytecodeGenerator.hpp"

BytecodeGenerator::BytecodeGenerator() : Environment(), EnvIndex(0) {
    Environment.Environments.emplace_back((LocalEnvironment) {L"XlangGlobalScope", 0});
};

BytecodeCommandArray BytecodeGenerator::Generate(AST &Target) {
    BytecodeCommandArray Result;
    switch (Target.Type) {
        case AST::TreeType::Primary: {
            switch (Target.Node.Kind) {
                case Lexer::TokenKind::Integer: {
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {(TypenameDerive) {(Typename) {Typename::TypenameKind::Integer}}});
                    Result.PushCommand({BytecodeCommand::Instruction::push_integer, (BytecodeOperandType) {
                            std::stol(Target.Node.Value)}});
                    break;
                }
                case Lexer::TokenKind::Decimal: {
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {(TypenameDerive) {(Typename) {Typename::TypenameKind::Decimal}}});
                    Result.PushCommand({BytecodeCommand::Instruction::push_decimal, (BytecodeOperandType) {
                            std::stof(Target.Node.Value)}});
                    break;
                }
                default: {
                    throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                    L"Unexpected primary token type.");
                }
            }
            break;
        }
        case AST::TreeType::Identifier: {
            /* Emulate the stack process. */
            Environment.EmuStack.StackFrames.back().PushItem((EmulateStack::Item) {GetTypeOfAST(Target)});

            /* It's already checked variable is or isn't exist, we don't need to check in here again.*/
            auto Iter = Environment.SearchSymbol(EnvIndex, Target.Node.Value);

            /* Duplicate value from original stack index. */
            Result.PushCommand({BytecodeCommand::Instruction::duplicate,
                                (BytecodeOperandType) {
                                        Environment.Environments[Iter.first].SymbolItem[Iter.second].StackIndex}});
            break;
        }
        case AST::TreeType::IndexExpression: {
            /* Merge left value of expression */
            Result.Merge(Generate(Target.Subtrees[0]));
            /* Merge subscript value of index expression */
            Result.Merge(Generate(Target.Subtrees[1]));

            TypenameDerive TypeOfExpression = GetTypeOfAST(Target);
            Environment.EmuStack.StackFrames.back().PopItem(2);
            Environment.EmuStack.StackFrames.back().PushItem((EmulateStack::Item){TypeOfExpression});
            Result.PushCommand({BytecodeCommand::Instruction::array_index, {}});
            break;
        }
        default: {
            Lexer::Token O = Target.GetFirstNotNullToken();
            throw BytecodeGenerateException(O.Line, O.Column,
                                            L"Unexpected AST type.");
        }
    }
    return Result;
}

TypenameDerive BytecodeGenerator::GetTypeOfAST(AST &Target) {
    TypenameDerive Result;
    switch (Target.Type) {
        case AST::TreeType::Primary: {
            switch (Target.Node.Kind) {
                case Lexer::TokenKind::Integer: {
                    Result.OriginalType = Typename(Typename::TypenameKind::Integer);
                    break;
                }
                case Lexer::TokenKind::Decimal: {
                    Result.OriginalType = Typename(Typename::TypenameKind::Decimal);
                    break;
                }
                case Lexer::TokenKind::String: {
                    Result.OriginalType = Typename(Typename::TypenameKind::String);
                    break;
                }
                default: {
                    throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                    L"Unexpected primary token type.");
                }
            }
            break;
        }
        case AST::TreeType::Identifier: {
            auto Val = Environment.SearchSymbol(EnvIndex, Target.Node.Value);
            if (Val.second != -1) {
                Result = Environment.Environments[Val.first].SymbolItem[Val.second].Type;
            } else {
                throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                L"Undefined symbol : " + Target.Node.Value);
            }
            break;
        }
        case AST::TreeType::IndexExpression: {
            auto Val = GetTypeOfAST(Target.Subtrees[0]);
            if (Val.OriginalType.Kind == Typename::TypenameKind::Array) {
                Result = Val;
                Result.ArrayDimensionCount--;
                if (!Result.ArrayDimensionCount) {
                    Result.Kind = TypenameDerive::DeriveKind::NoDerive;
                }
            }
            break;
        }
        case AST::TreeType::AdditionExpression:
        case AST::TreeType::MultiplicationExpression: {
            auto Left = GetTypeOfAST(Target.Subtrees[0]);
            auto Right = GetTypeOfAST(Target.Subtrees[0]);
            if (Left.Kind == TypenameDerive::DeriveKind::NoDerive and
                Right.Kind == TypenameDerive::DeriveKind::NoDerive) {
                if (Left.OriginalType.Kind == Typename::TypenameKind::String)
                    Result = Left;
                if (Left.OriginalType.Kind == Typename::TypenameKind::Decimal)
                    Result = Left;
                if (Right.OriginalType.Kind == Typename::TypenameKind::Decimal)
                    Result = Right;
            }
            break;
        }
        case AST::TreeType::LogicExpression:
        case AST::TreeType::EqualExpression:
        case AST::TreeType::ComparingExpression: {
            Result.OriginalType = (Typename) {Typename::TypenameKind::Boolean};
            Result.Kind = TypenameDerive::DeriveKind::NoDerive;
            Result.ArrayDimensionCount = 0;
            Result.FunctionArgumentsList = {};
            break;
        }
        case AST::TreeType::BinaryExpression: {
            Result.OriginalType = (Typename) {Typename::TypenameKind::Integer};
            Result.Kind = TypenameDerive::DeriveKind::NoDerive;
            Result.ArrayDimensionCount = 0;
            Result.FunctionArgumentsList = {};
            break;
        }
        case AST::TreeType::TypeSpecifier: {
            if (Target.Subtrees[0].Node.Value == L"Integer") {
                Result.OriginalType = (Typename) {Typename::TypenameKind::Integer};
            } else if (Target.Subtrees[0].Node.Value == L"Decimal") {
                Result.OriginalType = (Typename) {Typename::TypenameKind::Decimal};
            } else if (Target.Subtrees[0].Node.Value == L"Boolean") {
                Result.OriginalType = (Typename) {Typename::TypenameKind::Boolean};
            } else if (Target.Subtrees[0].Node.Value == L"String") {
                Result.OriginalType = (Typename) {Typename::TypenameKind::String};
            }
            if (Target.Subtrees[1].TypeSpecifierArrayDimensionsCount) {
                Result.Kind = TypenameDerive::DeriveKind::ArrayDerive;
                Result.ArrayDimensionCount = Target.TypeSpecifierArrayDimensionsCount;
                Result.FunctionArgumentsList = {};
            } else {
                Result.Kind = TypenameDerive::DeriveKind::NoDerive;
                Result.ArrayDimensionCount = 0;
            }
            break;
        }
        default: {
            Lexer::Token O = Target.GetFirstNotNullToken();
            throw BytecodeGenerateException(O.Line, O.Column,
                                            L"Unexpected AST type.");
        }
    }
    return Result;
}

#pragma clang diagnostic pop