//
// Created by Jerry Chou on 2022/4/2.
//

#include "BytecodeGenerator.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

BytecodeGenerator::BytecodeGenerator() : Environment(), EnvIndex(0) {
    Environment.Environments.emplace_back((LocalEnvironment) {L"XlangGlobalScope", 0});
}

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
                case Lexer::TokenKind::Boolean: {
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {(TypenameDerive) {(Typename) {Typename::TypenameKind::Boolean}}});

                    Result.PushCommand({BytecodeCommand::Instruction::push_boolean,
                                        (BytecodeOperandType) {Target.Node.Value == L"True"}});
                    break;
                }
                case Lexer::TokenKind::String: {
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {(TypenameDerive) {(Typename) {Typename::TypenameKind::String}}});

                    Result.PushCommand({BytecodeCommand::Instruction::get_const_string,
                                        (BytecodeOperandType) {
                                                Environment.PushConstantItem((ConstantPoolItem) {Target.Node.Value})}});
                    break;
                }
                default: {
                    throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                    L"Unexpected primary token type.");
                }
            }
            break;
        }

        case AST::TreeType::Identifier:
        case AST::TreeType::FunctionCallingExpression:
        case AST::TreeType::MemberExpression: {
            /* use ParseMemberExpression to generate codes for these expressions */
            XClassIndexType ParseTo = -1;
            Result.Merge(ParseMemberExpression(Target, false, ParseTo));
            break;
        }

        case AST::TreeType::IndexExpression: {
            /* Merge left value of expression */
            Result.Merge(Generate(Target.Subtrees[0]));
            /* Merge subscript value of index expression */
            Result.Merge(Generate(Target.Subtrees[1]));

            TypenameDerive TypeOfExpression = GetTypeOfAST(Target);
            Environment.EmuStack.StackFrames.back().PopItem(2);
            Environment.EmuStack.StackFrames.back().PushItem((EmulateStack::Item) {TypeOfExpression});
            Result.PushCommand({BytecodeCommand::Instruction::array_index, {}});
            break;
        }
        case AST::TreeType::NegativeExpression: {
            TypenameDerive TypeOfExpression = GetTypeOfAST(Target.Subtrees[0]);
            if (TypeOfExpression.Kind != TypenameDerive::DeriveKind::NoDerive) {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Unexpected expression type : The type of the expression is not NoDerive type.");
            }
            Result.Merge(Generate(Target.Subtrees[0]));
            switch (TypeOfExpression.OriginalType.Kind) {
                case Typename::TypenameKind::Integer: {
                    Environment.EmuStack.StackFrames.back().PopItem(1);
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {(TypenameDerive) {(Typename) {Typename::TypenameKind::Integer}}});

                    Result.PushCommand({BytecodeCommand::Instruction::negate_integer, {}});
                    break;
                }
                case Typename::TypenameKind::Decimal: {
                    Environment.EmuStack.StackFrames.back().PopItem(1);
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {(TypenameDerive) {(Typename) {Typename::TypenameKind::Decimal}}});

                    Result.PushCommand({BytecodeCommand::Instruction::negate_decimal, {}});
                    break;
                }
                case Typename::TypenameKind::Boolean: {
                    Environment.EmuStack.StackFrames.back().PopItem(1);
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {(TypenameDerive) {(Typename) {Typename::TypenameKind::Boolean}}});

                    Result.PushCommand({BytecodeCommand::Instruction::negate_boolean, {}});
                    break;
                }
                default: {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Unexpected expression type : The type of expression is not integer or decimal");
                }
            }
            break;
        }

        case AST::TreeType::IncrementExpression: {
            /* do type check */
            TypenameDerive TypeOfExpression = GetTypeOfAST(Target);

            /* generate codes */
            XIndexType ParseTo = -1;
            Result.Merge(ParseMemberExpression(Target.Subtrees[0], false, ParseTo));

            /* do not need to throw exception, we did type check before. */
            if (TypeOfExpression.Kind == TypenameDerive::DeriveKind::FunctionDerive) {
                break;
            } else if (TypeOfExpression.Kind == TypenameDerive::DeriveKind::NoDerive) {
                if (TypeOfExpression.OriginalType.Kind == Typename::TypenameKind::Integer) {
                    Result.PushCommand(
                            {BytecodeCommand::Instruction::add_integer, (BytecodeOperandType) {(XInteger) 1}});
                } else if (TypeOfExpression.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                    Result.PushCommand(
                            {BytecodeCommand::Instruction::add_decimal, (BytecodeOperandType) {(XDecimal) 1}});
                }
            }

            /* generate assignment codes */
            Result.Merge(ParseMemberExpression(Target.Subtrees[0], true, ParseTo));

            Environment.EmuStack.StackFrames.back().PopItem(1);
            Environment.EmuStack.StackFrames.back().PushItem(
                    (EmulateStack::Item) {TypeOfExpression});
        }
        case AST::TreeType::DecrementExpression: {
            /* do type check */
            TypenameDerive TypeOfExpression = GetTypeOfAST(Target);

            /* generate codes */
            XIndexType ParseTo = -1;
            Result.Merge(ParseMemberExpression(Target.Subtrees[0], false, ParseTo));

            /* do not need to throw exception, we did type check before. */
            if (TypeOfExpression.Kind == TypenameDerive::DeriveKind::FunctionDerive) {
                break;
            } else if (TypeOfExpression.Kind == TypenameDerive::DeriveKind::NoDerive) {
                if (TypeOfExpression.OriginalType.Kind == Typename::TypenameKind::Integer) {
                    Result.PushCommand({BytecodeCommand::Instruction::decrement_integer, {}});
                } else if (TypeOfExpression.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                    Result.PushCommand({BytecodeCommand::Instruction::decrement_decimal, {}});
                }
            }

            /* generate assignment codes */
            Result.Merge(ParseMemberExpression(Target.Subtrees[0], false, ParseTo));

            Environment.EmuStack.StackFrames.back().PopItem(1);
            Environment.EmuStack.StackFrames.back().PushItem(
                    (EmulateStack::Item) {TypeOfExpression});
        }

        case AST::TreeType::AdditionExpression:
        case AST::TreeType::MultiplicationExpression: {
            /* do type check */
            TypenameDerive LeftTree = GetTypeOfAST(Target.Subtrees[0]);
            TypenameDerive RightTree = GetTypeOfAST(Target.Subtrees[2]);
            TypenameDerive TypeOfAST = GetTypeOfAST(Target);

            /* generate codes */
            /* fix priority of expression subtrees */
            Result.Merge(Generate(Target.Subtrees[0]));

            /* do type casting for right tree */
            if (LeftTree.OriginalType.Kind == Typename::TypenameKind::Integer and
                TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                Result.PushCommand({BytecodeCommand::Instruction::int_to_deci, {}});
            } else if (LeftTree.OriginalType.Kind == Typename::TypenameKind::Boolean and
                       TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                Result.PushCommand({BytecodeCommand::Instruction::bool_to_deci, {}});
            } else if (LeftTree.OriginalType.Kind == Typename::TypenameKind::Boolean and
                       TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Integer) {
                Result.PushCommand({BytecodeCommand::Instruction::bool_to_int, {}});
            }

            Result.Merge(Generate(Target.Subtrees[2]));

            /* do type casting for left tree */
            if (RightTree.OriginalType.Kind == Typename::TypenameKind::Integer and
                TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                Result.PushCommand({BytecodeCommand::Instruction::int_to_deci, {}});
            } else if (RightTree.OriginalType.Kind == Typename::TypenameKind::Boolean and
                       TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                Result.PushCommand({BytecodeCommand::Instruction::bool_to_deci, {}});
            } else if (RightTree.OriginalType.Kind == Typename::TypenameKind::Boolean and
                       TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Integer) {
                Result.PushCommand({BytecodeCommand::Instruction::bool_to_int, {}});
            }

            switch (Target.Subtrees[1].Node.Kind) {
                case Lexer::TokenKind::Asterisk: {
                    if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Integer)
                        Result.PushCommand({BytecodeCommand::Instruction::mul_integer, {}});
                    else if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal)
                        Result.PushCommand({BytecodeCommand::Instruction::mul_decimal, {}});
                    else;
                    break;
                }
                case Lexer::TokenKind::Slash: {
                    if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Integer)
                        Result.PushCommand({BytecodeCommand::Instruction::div_integer, {}});
                    else if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal)
                        Result.PushCommand({BytecodeCommand::Instruction::div_decimal, {}});
                    else;
                    break;
                }
                case Lexer::TokenKind::PercentSign: {
                    if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Integer)
                        Result.PushCommand({BytecodeCommand::Instruction::mod_integer, {}});
                    else if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal)
                        Result.PushCommand({BytecodeCommand::Instruction::mod_decimal, {}});
                    else;
                    break;
                }
                case Lexer::TokenKind::Plus: {
                    if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Integer)
                        Result.PushCommand({BytecodeCommand::Instruction::add_integer, {}});
                    else if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal)
                        Result.PushCommand({BytecodeCommand::Instruction::add_decimal, {}});
                    else;
                    break;
                }
                case Lexer::TokenKind::Minus: {
                    if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Integer)
                        Result.PushCommand({BytecodeCommand::Instruction::div_integer, {}});
                    else if (TypeOfAST.OriginalType.Kind == Typename::TypenameKind::Decimal)
                        Result.PushCommand({BytecodeCommand::Instruction::div_decimal, {}});
                    else;
                    break;
                }
                default: {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Internal Error");
                }
            }

            Environment.EmuStack.StackFrames.back().PopItem(2);
            Environment.EmuStack.StackFrames.back().PushItem((EmulateStack::Item) {TypeOfAST});
            break;
        }
        case AST::TreeType::BinaryMoveExpression: {
            /* do type check */
            TypenameDerive LeftTree = GetTypeOfAST(Target.Subtrees[0]);
            TypenameDerive RightTree = GetTypeOfAST(Target.Subtrees[2]);
            TypenameDerive TypeOfAST = GetTypeOfAST(Target);

            /* generate codes */
            Result.Merge(Generate(Target.Subtrees[0]));
            Result.Merge(Generate(Target.Subtrees[2]));

            switch (Target.Subtrees[1].Node.Kind) {
                case Lexer::TokenKind::BinaryLeftMove: {
                    Result.PushCommand({BytecodeCommand::Instruction::binary_lm, {}});
                    break;
                }
                case Lexer::TokenKind::BinaryRightMove: {
                    Result.PushCommand({BytecodeCommand::Instruction::binary_rm, {}});
                    break;
                }
                default: {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Internal Error");
                }
            }

            Environment.EmuStack.StackFrames.back().PopItem(2);
            Environment.EmuStack.StackFrames.back().PushItem((EmulateStack::Item) {TypeOfAST});

            break;
        }
        default: {
            Lexer::Token O = Target.GetFirstNotNullToken();
            throw BytecodeGenerateException(O.Line, O.Column,
                                            L"Generate: Unexpected AST type.");
        }
    }
    return Result;
}

TypenameDerive BytecodeGenerator::GetTypeOfAST(AST &Target) {
    /* This function CANNOT CHANGE ANYTHING from emulate stack!!! */
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
                case Lexer::TokenKind::Boolean: {
                    Result.OriginalType = Typename(Typename::TypenameKind::Boolean);
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
        case AST::TreeType::NegativeExpression: {
            TypenameDerive TypeOfExpression = GetTypeOfAST(Target.Subtrees[0]);
            if (TypeOfExpression.Kind != TypenameDerive::DeriveKind::NoDerive) {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Unexpected expression type : The type of the expression is not NoDerive type.");
            }
            switch (TypeOfExpression.OriginalType.Kind) {
                case Typename::TypenameKind::Integer:
                case Typename::TypenameKind::Decimal:
                case Typename::TypenameKind::Boolean: {
                    Result = TypeOfExpression;
                    break;
                }

                default: {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Unexpected expression type : The type of expression is not integer or decimal");
                }
            }
        }
        case AST::TreeType::IncrementExpression:
        case AST::TreeType::DecrementExpression: {
            TypenameDerive TypeOfExpression = GetTypeOfAST(Target.Subtrees[0]);
            if (TypeOfExpression.Kind == TypenameDerive::DeriveKind::FunctionDerive) {
                break;
            } else if (TypeOfExpression.Kind == TypenameDerive::DeriveKind::NoDerive) {
                if (TypeOfExpression.OriginalType.Kind == Typename::TypenameKind::Integer or
                    TypeOfExpression.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                    Result = TypeOfExpression;
                    break;
                } else {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Unexpected expression type : The type of expression is not integer or decimal");
                }
            } else {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Unexpected expression type : The type of the expression is not NoDerive type.");
            }
        }
        case AST::TreeType::AdditionExpression:
        case AST::TreeType::MultiplicationExpression: {
            auto Left = GetTypeOfAST(Target.Subtrees[0]);
            auto Right = GetTypeOfAST(Target.Subtrees[2]);
            if (Left.Kind == TypenameDerive::DeriveKind::NoDerive and
                Right.Kind == TypenameDerive::DeriveKind::NoDerive) {
                if (Left.OriginalType.Kind == Typename::TypenameKind::Decimal)
                    Result = Left;
                if (Right.OriginalType.Kind == Typename::TypenameKind::Decimal)
                    Result = Right;
            }
            break;
        }
        case AST::TreeType::BinaryMoveExpression: {
            auto Left = GetTypeOfAST(Target.Subtrees[0]);
            auto Right = GetTypeOfAST(Target.Subtrees[2]);
            if (Left.Kind == TypenameDerive::DeriveKind::NoDerive and
                Right.Kind == TypenameDerive::DeriveKind::NoDerive) {
                if (Left.OriginalType.Kind != Typename::TypenameKind::Integer or
                    Right.OriginalType.Kind != Typename::TypenameKind::Integer) {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Unexpected expression type : BinaryMoveExpression expected a Integer type.");
                }
                Result = Left;
            } else {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Unexpected expression type : The type of the expression is not NoDerive type.");
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
                                            L"GetTypeOfAST: Unexpected AST type.");
        }
    }
    return Result;
}

BytecodeCommandArray
BytecodeGenerator::ParseMemberExpression(AST &Target, bool EndWithAssignment, XClassIndexType &ParseTo) {
    BytecodeCommandArray Result{};
    switch (Target.Type) {
        case AST::TreeType::MemberExpression: {
            /* use EndWithAssignment in last expression */
            ParseMemberExpression(Target.Subtrees[0], false, ParseTo);
            if (Environment.EmuStack.StackFrames.back().Items.back().Kind == EmulateStack::Item::ItemKind::Temp and
                Environment.EmuStack.StackFrames.back().Items.back().Temp.Kind ==
                TypenameDerive::DeriveKind::NoDerive and
                Environment.EmuStack.StackFrames.back().Items.back().Temp.OriginalType.Kind ==
                Typename::TypenameKind::Class) {
                ParseTo = Environment.EmuStack.StackFrames.back().Items.back().Temp.OriginalType.ClassIndexInGlobalEnvironment;
            } else {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"LeftExpression is not class-type.");
            }
            ParseMemberExpression(Target.Subtrees[1], EndWithAssignment, ParseTo);
            break;
        }
        case AST::TreeType::Identifier: {
            if (ParseTo == -1) {
                /* Is not searching, find the way to start */
                /* do type checking */
                TypenameDerive TypeOfAST = GetTypeOfAST(Target);
                /* Emulate the stack process. */
                Environment.EmuStack.StackFrames.back().PushItem((EmulateStack::Item) {TypeOfAST});

                auto Val = Environment.SearchSymbol(EnvIndex, Target.Node.Value);
                Result.PushCommand({EndWithAssignment ? BytecodeCommand::Instruction::store
                                                      : BytecodeCommand::Instruction::duplicate, (BytecodeOperandType) {
                        Environment.Environments[Val.first].SymbolItem[Val.second].StackIndex}});
            } else {
                /* Is searching, find fields in XlangClass structure */
                if (Environment.ClassPool[ParseTo].Members.count(Target.Node.Value)) {
                    /* Field exist */
                    Environment.EmuStack.StackFrames.back().PopItem(1);
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {Environment.ClassPool[ParseTo].Members[Target.Node.Value]});
                    /* get field */
                    Result.PushCommand({EndWithAssignment ? BytecodeCommand::Instruction::put_field
                                                          : BytecodeCommand::Instruction::get_field,
                                        (BytecodeOperandType) {HashLib::StringHash(Target.Node.Value)}});
                } else {
                    /* Field doesn't exist, make exception */
                    throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                    Environment.ClassPool[ParseTo].ClassName +
                                                    L" doesn't contains a field which is named " + Target.Node.Value);
                }
            }
            break;
        }
        case AST::TreeType::FunctionCallingExpression: {
            if (EndWithAssignment) {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Cannot assign for a right-value expression.");
            }
            if (ParseTo == -1) {
                /* Is not searching, find methods in root field */
                auto Iter = Environment.SearchSymbol(EnvIndex, Target.Node.Value);
                if (Iter.second != -1) {
                    if (Environment.Environments[Iter.first].SymbolItem[Iter.second].Type.Kind !=
                        TypenameDerive::DeriveKind::FunctionDerive) {
                        throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                        Target.Node.Value + L" isn't a function derive.");
                    }

                    /* check function arguments count */
                    if (Target.Subtrees[1].Subtrees.size() !=
                        Environment.Environments[Iter.first].SymbolItem[Iter.second].Type.FunctionArgumentsList.size()) {
                        Lexer::Token O = Target.GetFirstNotNullToken();
                        throw BytecodeGenerateException(O.Line, O.Column,
                                                        O.Value +
                                                        L": Cannot call a function with different arguments count.");
                    }

                    /* generate codes for arguments */
                    for (XIndexType Index = 0; Index < Target.Subtrees[1].Subtrees.size(); Index++) {
                        /* Not the same type */
                        if (GetTypeOfAST(Target.Subtrees[1].Subtrees[Index]) !=
                            Environment.Environments[Iter.first].SymbolItem[Iter.second].Type.FunctionArgumentsList[Index]) {
                            Lexer::Token O = Target.GetFirstNotNullToken();
                            throw BytecodeGenerateException(O.Line, O.Column,
                                                            L"Function arguments value is not the same.");
                        }
                        Result.Merge(Generate(Target.Subtrees[1].Subtrees[Index]));
                    }

                    /* push function */
                    Environment.EmuStack.StackFrames.back().PopItem(1);
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {Environment.Environments[Iter.first].SymbolItem[Iter.second].Type});

                    Result.PushCommand({BytecodeCommand::Instruction::push_function,
                                        (BytecodeOperandType) {HashLib::StringHash(
                                                Environment.FunctionPool[Environment.Environments[Iter.first].SymbolItem[Iter.second].Type.FunctionIndexInPool].FunctionName
                                        )}});

                    /* generate invoke command */
                    Result.PushCommand({BytecodeCommand::Instruction::invoke_function,
                                        (BytecodeOperandType) {(XIndexType) Target.Subtrees[1].Subtrees.size()}});

                    /* restore emulation, store result into stack */
                    Environment.EmuStack.StackFrames.back().PopItem(Target.Subtrees[1].Subtrees.size() + 1);
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {
                                    Environment.FunctionPool[Environment.Environments[Iter.first].SymbolItem[Iter.second].Type.FunctionIndexInPool].ReturnValueType});
                } else {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Function pool doesn't contains a function which is named " +
                                                    O.Value);
                }
            } else {
                /* Is searching, find methods with ParseTo */
                /* methods will be named with PackageName$ClassName$MethodName */
                if (Environment.ClassPool[ParseTo].Members.count(Target.Node.Value)) {
                    auto &Func = Environment.ClassPool[ParseTo].Members[Target.Node.Value];
                    if (Func.Kind != TypenameDerive::DeriveKind::FunctionDerive) {
                        throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                        Target.Node.Value + L" isn't a function derive.");
                    }
                    /* check function arguments count */
                    if (Target.Subtrees[1].Subtrees.size() != Func.FunctionArgumentsList.size()) {
                        Lexer::Token O = Target.GetFirstNotNullToken();
                        throw BytecodeGenerateException(O.Line, O.Column,
                                                        O.Value +
                                                        L": Cannot call a function with different arguments count.");
                    }

                    /* generate codes for arguments */
                    for (XIndexType Index = 0; Index < Target.Subtrees[1].Subtrees.size(); Index++) {
                        /* Not the same type */
                        if (GetTypeOfAST(Target.Subtrees[1].Subtrees[Index]) != Func.FunctionArgumentsList[Index]) {
                            Lexer::Token O = Target.GetFirstNotNullToken();
                            throw BytecodeGenerateException(O.Line, O.Column,
                                                            L"Function arguments value is not the same.");
                        }
                        Result.Merge(Generate(Target.Subtrees[1].Subtrees[Index]));
                    }

                    /* push function */
                    Environment.EmuStack.StackFrames.back().PopItem(1);
                    Environment.EmuStack.StackFrames.back().PushItem((EmulateStack::Item) {Func});

                    Result.PushCommand({BytecodeCommand::Instruction::push_function,
                                        (BytecodeOperandType) {HashLib::StringHash(
                                                Environment.FunctionPool[Func.FunctionIndexInPool].FunctionName
                                        )}});

                    /* generate invoke command */
                    Result.PushCommand({BytecodeCommand::Instruction::invoke_function,
                                        (BytecodeOperandType) {(XIndexType) Target.Subtrees[1].Subtrees.size()}});

                    /* restore emulation, store result into stack */
                    Environment.EmuStack.StackFrames.back().PopItem(Target.Subtrees[1].Subtrees.size() + 1);
                    Environment.EmuStack.StackFrames.back().PushItem(
                            (EmulateStack::Item) {Environment.FunctionPool[Func.FunctionIndexInPool].ReturnValueType});
                } else {
                    throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                    Environment.ClassPool[ParseTo].ClassName +
                                                    L" doesn't contains a field which is named " + Target.Node.Value);
                }
            }
            break;
        }
        default: {
            Lexer::Token O = Target.GetFirstNotNullToken();
            throw BytecodeGenerateException(O.Line, O.Column,
                                            L"ParseMemberExpression: Unexpected AST type.");
        }
    }
    return Result;
}

#pragma clang diagnostic pop