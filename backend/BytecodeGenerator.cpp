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
        case AST::TreeType::TypeCastingExpression: {
            /* do type checking */
            TypenameDerive TypeOfAST = GetTypeOfAST(Target);
            TypenameDerive Left = GetTypeOfAST(Target.Subtrees[0]);
            TypenameDerive Right = GetTypeOfAST(Target.Subtrees[1]);

            if (Left.Kind == TypenameDerive::DeriveKind::NoDerive and
                Right.Kind == TypenameDerive::DeriveKind::NoDerive) {
                Result.Merge(Generate(Target.Subtrees[0]));
                switch (Left.OriginalType.Kind) {
                    case Typename::TypenameKind::Integer: {
                        switch (Right.OriginalType.Kind) {
                            case Typename::TypenameKind::Decimal: {
                                Result.PushCommand({BytecodeCommand::Instruction::int_to_deci, {}});
                                break;
                            }
                            case Typename::TypenameKind::Boolean: {
                                Result.PushCommand({BytecodeCommand::Instruction::int_to_bool, {}});
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    case Typename::TypenameKind::Decimal: {
                        switch (Right.OriginalType.Kind) {
                            case Typename::TypenameKind::Integer: {
                                Result.PushCommand({BytecodeCommand::Instruction::deci_to_int, {}});
                                break;
                            }
                            case Typename::TypenameKind::Boolean: {
                                Result.PushCommand({BytecodeCommand::Instruction::deci_to_bool, {}});
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    case Typename::TypenameKind::Boolean: {
                        switch (Right.OriginalType.Kind) {
                            case Typename::TypenameKind::Integer: {
                                Result.PushCommand({BytecodeCommand::Instruction::bool_to_int, {}});
                                break;
                            }
                            case Typename::TypenameKind::Decimal: {
                                Result.PushCommand({BytecodeCommand::Instruction::bool_to_deci, {}});
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    case Typename::TypenameKind::Class: {
                        /* Goal: Finish the class upcast and downcast*/
                        if (Right.OriginalType.Kind == Typename::TypenameKind::Class) {
                            /* up-cast */
                            /* 现在只能向上一层的父类进行转换 不能向父类的父类进行转换 */
                            if (Environment.ClassPool[Left.OriginalType.ClassIndexInGlobalEnvironment].Extends ==
                                Right.OriginalType.ClassIndexInGlobalEnvironment) {
                                Result.PushCommand({BytecodeCommand::Instruction::up_cast,
                                                    (BytecodeOperandType) {
                                                            (XClassIndexType) Right.OriginalType.ClassIndexInGlobalEnvironment}});
                            } else if (
                                    Environment.ClassPool[Right.OriginalType.ClassIndexInGlobalEnvironment].Extends ==
                                    Left.OriginalType.ClassIndexInGlobalEnvironment) {
                                Result.PushCommand({BytecodeCommand::Instruction::down_cast,
                                                    (BytecodeOperandType) {
                                                            (XClassIndexType) Right.OriginalType.ClassIndexInGlobalEnvironment}});
                            }
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
                Environment.EmuStack.StackFrames.back().PopItem(1);
                Environment.EmuStack.StackFrames.back().PushItem(
                        (EmulateStack::Item) {TypeOfAST});
            } else {
                /* never get in */
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
                            {BytecodeCommand::Instruction::push_integer, (BytecodeOperandType) {(XInteger) 1}});
                    Result.PushCommand(
                            {BytecodeCommand::Instruction::add_integer, (BytecodeOperandType) {(XInteger) 1}});
                } else if (TypeOfExpression.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                    Result.PushCommand(
                            {BytecodeCommand::Instruction::push_decimal, (BytecodeOperandType) {(XDecimal) 1}});
                    Result.PushCommand(
                            {BytecodeCommand::Instruction::add_decimal, (BytecodeOperandType) {(XDecimal) 1}});
                } else {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Generate: IncrementExpression : Unsupported value type.");
                }
            }

            /* generate assignment codes */
            Result.Merge(ParseMemberExpression(Target.Subtrees[0], true, ParseTo));

            Environment.EmuStack.StackFrames.back().PopItem(1);
            Environment.EmuStack.StackFrames.back().PushItem(
                    (EmulateStack::Item) {TypeOfExpression});
            break;
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
                    Result.PushCommand(
                            {BytecodeCommand::Instruction::push_integer, (BytecodeOperandType) {(XInteger) -1}});
                    Result.PushCommand({BytecodeCommand::Instruction::add_integer, {}});
                } else if (TypeOfExpression.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                    Result.PushCommand(
                            {BytecodeCommand::Instruction::push_integer, (BytecodeOperandType) {(XDecimal) -1}});
                    Result.PushCommand({BytecodeCommand::Instruction::add_decimal, {}});
                }
            } else {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Generate: DecrementExpression : Unsupported value type.");
            }

            /* generate assignment codes */
            Result.Merge(ParseMemberExpression(Target.Subtrees[0], false, ParseTo));

            Environment.EmuStack.StackFrames.back().PopItem(1);
            Environment.EmuStack.StackFrames.back().PushItem(
                    (EmulateStack::Item) {TypeOfExpression});
            break;
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

        case AST::TreeType::BinaryExpression:
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
                case Lexer::TokenKind::BinaryXOR: {
                    Result.PushCommand({BytecodeCommand::Instruction::binary_xor, {}});
                    break;
                }
                case Lexer::TokenKind::BinaryOr: {
                    Result.PushCommand({BytecodeCommand::Instruction::binary_or, {}});
                    break;
                }
                case Lexer::TokenKind::BinaryAnd: {
                    Result.PushCommand({BytecodeCommand::Instruction::binary_and, {}});
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

        case AST::TreeType::LogicExpression:
        case AST::TreeType::EqualExpression:
        case AST::TreeType::ComparingExpression: {
            /* type checking */
            TypenameDerive TypeOfLeftTree = GetTypeOfAST(Target.Subtrees[0]);
            TypenameDerive TypeOfRightTree = GetTypeOfAST(Target.Subtrees[2]);
            if (TypeOfLeftTree.Kind == TypenameDerive::DeriveKind::NoDerive and
                TypeOfRightTree.Kind == TypenameDerive::DeriveKind::NoDerive) {
                if (TypeOfLeftTree.OriginalType.Kind == Typename::TypenameKind::Class) {
                    /* Finish the class comparing process */
                } else {
                    /* type casting for left tree */
                    Result.Merge(Generate(Target.Subtrees[0]));

                    if (TypeOfLeftTree.OriginalType.Kind == Typename::TypenameKind::Integer and
                        TypeOfRightTree.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                        Result.PushCommand({BytecodeCommand::Instruction::int_to_deci, {}});
                    } else if (TypeOfLeftTree.OriginalType.Kind == Typename::TypenameKind::Boolean and
                               TypeOfRightTree.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                        Result.PushCommand({BytecodeCommand::Instruction::bool_to_deci, {}});
                    }

                    /* type casting for right tree */
                    Result.Merge(Generate(Target.Subtrees[2]));

                    if (TypeOfRightTree.OriginalType.Kind == Typename::TypenameKind::Integer and
                        TypeOfLeftTree.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                        Result.PushCommand({BytecodeCommand::Instruction::int_to_deci, {}});
                    } else if (TypeOfRightTree.OriginalType.Kind == Typename::TypenameKind::Boolean and
                               TypeOfLeftTree.OriginalType.Kind == Typename::TypenameKind::Decimal) {
                        Result.PushCommand({BytecodeCommand::Instruction::bool_to_deci, {}});
                    }

                    switch (Target.Subtrees[1].Node.Kind) {
                        case Lexer::TokenKind::LessThanOrEqual: {
                            Result.PushCommand({BytecodeCommand::Instruction::logic_le, {}});
                            break;
                        }
                        case Lexer::TokenKind::LessThan: {
                            Result.PushCommand({BytecodeCommand::Instruction::logic_lt, {}});
                            break;
                        }
                        case Lexer::TokenKind::MoreThanOrEqual: {
                            Result.PushCommand({BytecodeCommand::Instruction::logic_me, {}});
                            break;
                        }
                        case Lexer::TokenKind::MoreThan: {
                            Result.PushCommand({BytecodeCommand::Instruction::logic_mt, {}});
                            break;
                        }
                        case Lexer::TokenKind::Equal: {
                            Result.PushCommand({BytecodeCommand::Instruction::binary_and, {}});
                            break;
                        }
                        case Lexer::TokenKind::NotEqual: {
                            Result.PushCommand({BytecodeCommand::Instruction::binary_xor, {}});
                            break;
                        }
                        case Lexer::TokenKind::LogicAnd: {
                            Result.PushCommand({BytecodeCommand::Instruction::logic_and, {}});
                            break;
                        }
                        case Lexer::TokenKind::LogicOr: {
                            Result.PushCommand({BytecodeCommand::Instruction::logic_or, {}});
                            break;
                        }
                        default: {
                            break;
                        }
                    }

                    Environment.EmuStack.StackFrames.back().PopItem(2);
                    Environment.EmuStack.StackFrames.back().PushItem((EmulateStack::Item) {GetTypeOfAST(Target)});
                }
            } else {
                /* cannot compare with a deriving type*/
            }
            break;
        }

        case AST::TreeType::VariableDefinition:
        case AST::TreeType::VariableDeclaration: {
            TypenameDerive CompiledObject = GetTypeOfAST(Target.Subtrees[1]);
            if (!Environment.PushSymbolItem(EnvIndex, Target.Subtrees[2].Node.Value, CompiledObject)) {
                throw BytecodeGenerateException(Target.GetFirstNotNullToken().Line,
                                                Target.GetFirstNotNullToken().Column,
                                                L"Cannot create a variable with the same name '" +
                                                Target.Subtrees[2].Node.Value + L"'");
            }

            auto &CreatedResult = Environment.Environments[EnvIndex].SymbolItem.back();

            for (auto &Subtree: Target.Subtrees[0].Subtrees) {
                if (Subtree.Type == AST::TreeType::AccessDescriptor) {
                    if (Subtree.Node.Value == L"public") {
                        CreatedResult.AccessDescriptor = SymbolTableItem::Access::Public;
                    } else if (Subtree.Node.Value == L"private") {
                        CreatedResult.AccessDescriptor = SymbolTableItem::Access::Private;
                    }
                } else if (Subtree.Type == AST::TreeType::VariableDescriptor) {
                    /* Finish variable description process */
                }
            }

            if (Target.Type == AST::TreeType::VariableDefinition) {
                XClassIndexType ParseTo = -1;
                if (GetTypeOfAST(Target.Subtrees[3]) != CompiledObject) {
                    Lexer::Token O = Target.GetFirstNotNullToken();
                    throw BytecodeGenerateException(O.Line, O.Column,
                                                    L"Generate: Cannot resolve variable definition of type different from declaration.");
                }
                Result.Merge(Generate(Target.Subtrees[3]));
                /* 堆栈平衡 */
                /* It generated a 'push' command.
                 * In fact, it's the place of the variable, so we don't need to generate an assignment command.
                 * This idea is sucks, but it works, actually.
                 */
                Environment.EmuStack.StackFrames.back().PopItem(1);

            } else {
                switch (CompiledObject.Kind) {
                    case TypenameDerive::DeriveKind::ArrayDerive:
                    case TypenameDerive::DeriveKind::FunctionDerive: {
                        Lexer::Token O = Target.GetFirstNotNullToken();
                        throw BytecodeGenerateException(O.Line, O.Column,
                                                        L"Generate: Cannot parse a object address declaration without a initial value.");
                    }
                    case TypenameDerive::DeriveKind::NoDerive: {
                        switch (CompiledObject.OriginalType.Kind) {
                            case Typename::TypenameKind::Integer: {
                                Result.PushCommand({BytecodeCommand::Instruction::push_integer,
                                                    (BytecodeOperandType) {(XInteger) 0}});
                                break;
                            }
                            case Typename::TypenameKind::Decimal: {
                                Result.PushCommand({BytecodeCommand::Instruction::push_decimal,
                                                    (BytecodeOperandType) {(XDecimal) 0}});
                                break;
                            }
                            case Typename::TypenameKind::Boolean: {
                                Result.PushCommand({BytecodeCommand::Instruction::push_boolean,
                                                    (BytecodeOperandType) {false}});
                                break;
                            }
                            case Typename::TypenameKind::String: {
                                Lexer::Token O = Target.GetFirstNotNullToken();
                                throw BytecodeGenerateException(O.Line, O.Column,
                                                                L"Generate: Cannot parse a object address declaration without a initial value.");
                            }
                            case Typename::TypenameKind::Class: {
                                Lexer::Token O = Target.GetFirstNotNullToken();
                                throw BytecodeGenerateException(O.Line, O.Column,
                                                                L"Generate: Cannot parse a object address declaration without a initial value.");
                            }
                        }
                        break;
                    }
                    case TypenameDerive::DeriveKind::InvalidTypename: {
                        break;
                    }
                }
            }
            break;
        }
        case AST::TreeType::AssignmentExpression: {
            TypenameDerive TypeOfVal = GetTypeOfAST(Target.Subtrees[0]);
            if (GetTypeOfAST(Target.Subtrees[2]) != TypeOfVal) {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Generate: Cannot assign to lvalue which is different from rvalue.");
            }
            if (Target.Subtrees[1].Node.Kind == Lexer::TokenKind::AssignSign) {
                XIndexType ParseTo = -1;
                Result.Merge(Generate(Target.Subtrees[2]));
                Result.Merge(ParseMemberExpression(Target.Subtrees[0], true, ParseTo));
                Environment.EmuStack.StackFrames.back().PopItem(1);
            } else if (TypeOfVal.Kind == TypenameDerive::DeriveKind::NoDerive) {
                Result.Merge(Generate(Target.Subtrees[0]));
                Result.Merge(Generate(Target.Subtrees[2]));
                switch (TypeOfVal.OriginalType.Kind) {
                    case Typename::TypenameKind::Integer: {
                        switch (Target.Subtrees[1].Node.Kind) {
                            case Lexer::TokenKind::AdditionAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::add_integer, {}});
                                break;
                            }
                            case Lexer::TokenKind::SubtractionAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::sub_integer, {}});
                                break;
                            }
                            case Lexer::TokenKind::MultiplicationAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::mul_integer, {}});
                                break;
                            }
                            case Lexer::TokenKind::DivisionAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::div_integer, {}});
                                break;
                            }
                            case Lexer::TokenKind::RemainderAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::mod_integer, {}});
                                break;
                            }
                            default: {
                                Lexer::Token O = Target.GetFirstNotNullToken();
                                throw BytecodeGenerateException(O.Line, O.Column,
                                                                L"Generate: Unsupported operations.");
                            }
                        }
                        Environment.EmuStack.StackFrames.back().PopItem(1); /* Laudai ye go. */
                        XIndexType ParseTo = -1;
                        Result.Merge(ParseMemberExpression(Target.Subtrees[0], true, ParseTo));
                        Environment.EmuStack.StackFrames.back().PopItem(1); /* Tsyunbeu tantse. */
                        break;
                    }
                    case Typename::TypenameKind::Decimal: {
                        switch (Target.Subtrees[1].Node.Kind) {
                            case Lexer::TokenKind::AdditionAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::add_decimal, {}});
                                break;
                            }
                            case Lexer::TokenKind::SubtractionAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::sub_decimal, {}});
                                break;
                            }
                            case Lexer::TokenKind::MultiplicationAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::mul_decimal, {}});
                                break;
                            }
                            case Lexer::TokenKind::DivisionAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::div_decimal, {}});
                                break;
                            }
                            case Lexer::TokenKind::RemainderAssignment: {
                                Result.PushCommand({BytecodeCommand::Instruction::mod_decimal, {}});
                                break;
                            }
                            default: {
                                Lexer::Token O = Target.GetFirstNotNullToken();
                                throw BytecodeGenerateException(O.Line, O.Column,
                                                                L"Generate: Unsupported operations.");
                            }
                        }
                        Environment.EmuStack.StackFrames.back().PopItem(1); /* Laudai ye go. */
                        XIndexType ParseTo = -1;
                        Result.Merge(ParseMemberExpression(Target.Subtrees[0], true, ParseTo));
                        Environment.EmuStack.StackFrames.back().PopItem(1); /* Tsyunbeu tantse. */
                        break;
                    }
                    case Typename::TypenameKind::Boolean:
                    case Typename::TypenameKind::String:
                    case Typename::TypenameKind::Class: {
                        Lexer::Token O = Target.GetFirstNotNullToken();
                        throw BytecodeGenerateException(O.Line, O.Column,
                                                        L"Generate: Unsupported operations.");
                    }
                }
            } else if (TypeOfVal.Kind == TypenameDerive::DeriveKind::FunctionDerive) {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Generate: Unsupported operations.");
            } else if (TypeOfVal.Kind == TypenameDerive::DeriveKind::ArrayDerive) {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"Generate: Unsupported operations.");
            }
            break;
        }
        case AST::TreeType::CodeBlockStatement: {
            Environment.CreateInnerBlockFrame(EnvIndex);
            for (auto &Statement: Target.Subtrees) {
                Result.Merge(Generate(Statement));
                if (Statement.Type == AST::TreeType::IncrementExpression or
                    Statement.Type == AST::TreeType::DecrementExpression or
                    Statement.Type == AST::TreeType::AdditionExpression or
                    Statement.Type == AST::TreeType::MultiplicationExpression or
                    Statement.Type == AST::TreeType::BinaryExpression or
                    Statement.Type == AST::TreeType::BinaryMoveExpression or
                    Statement.Type == AST::TreeType::LogicExpression or
                    Statement.Type == AST::TreeType::EqualExpression or
                    Statement.Type == AST::TreeType::ComparingExpression or
                    Statement.Type == AST::TreeType::NegativeExpression or
                    Statement.Type == AST::TreeType::Primary or
                    Statement.Type == AST::TreeType::Identifier or
                    Statement.Type == AST::TreeType::IndexExpression or
                    Statement.Type == AST::TreeType::MemberExpression or
                    Statement.Type == AST::TreeType::FunctionCallingExpression or
                    Statement.Type == AST::TreeType::TypeCastingExpression) {
                    Result.PushCommand({BytecodeCommand::Instruction::pop_value, {}});
                    Environment.EmuStack.StackFrames.back().PopItem(1);
                }
            }
            Environment.LeaveInnerBlockFrame(EnvIndex);
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
                                                    L"GetTypeOfAST: Unexpected primary token type.");
                }
            }
            break;
        }
        case AST::TreeType::TypeCastingExpression: {
            TypenameDerive Left = GetTypeOfAST(Target.Subtrees[0]);
            TypenameDerive Right = GetTypeOfAST(Target.Subtrees[1]);
            if (Left == Right) {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"GetTypeOfAST: Cannot cast to a type as the same as the value.");
            }
            if (Left.Kind == TypenameDerive::DeriveKind::NoDerive and
                Right.Kind == TypenameDerive::DeriveKind::NoDerive) {
                switch (Left.OriginalType.Kind) {
                    case Typename::TypenameKind::Integer: {
                        switch (Right.OriginalType.Kind) {
                            case Typename::TypenameKind::String:
                            case Typename::TypenameKind::Class: {
                                Lexer::Token O = Target.GetFirstNotNullToken();
                                throw BytecodeGenerateException(O.Line, O.Column,
                                                                L"GetTypeOfAST: Cannot parse integer value to string or class.");
                            }
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    case Typename::TypenameKind::Decimal: {
                        switch (Right.OriginalType.Kind) {
                            case Typename::TypenameKind::String:
                            case Typename::TypenameKind::Class: {
                                Lexer::Token O = Target.GetFirstNotNullToken();
                                throw BytecodeGenerateException(O.Line, O.Column,
                                                                L"GetTypeOfAST: Cannot parse decimal value to string or class.");
                            }
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    case Typename::TypenameKind::Boolean: {
                        switch (Right.OriginalType.Kind) {
                            case Typename::TypenameKind::String:
                            case Typename::TypenameKind::Class: {
                                Lexer::Token O = Target.GetFirstNotNullToken();
                                throw BytecodeGenerateException(O.Line, O.Column,
                                                                L"GetTypeOfAST: Cannot parse boolean value to string or class.");
                            }
                            default: {
                                break;
                            }
                        }
                         break;
                    }
                    case Typename::TypenameKind::String: {
                        Lexer::Token O = Target.GetFirstNotNullToken();
                        throw BytecodeGenerateException(O.Line, O.Column,
                                                        L"GetTypeOfAST: Cannot parse string value to integer, boolean, decimal or class.");
                    }
                    case Typename::TypenameKind::Class: {
                        /* Goal: Finish the class upcast and downcast*/
                        if (Right.OriginalType.Kind == Typename::TypenameKind::Class) {
                            /* up-cast */
                            /* 现在只能向上一层的父类进行转换 不能向父类的父类进行转换 */
                            if (Environment.ClassPool[Left.OriginalType.ClassIndexInGlobalEnvironment].Extends ==
                                Right.OriginalType.ClassIndexInGlobalEnvironment) {
                                /* do nothing */
                            } else if (
                                    Environment.ClassPool[Right.OriginalType.ClassIndexInGlobalEnvironment].Extends ==
                                    Left.OriginalType.ClassIndexInGlobalEnvironment) {
                                /* do nothing */
                            } else {
                                Lexer::Token O = Target.GetFirstNotNullToken();
                                throw BytecodeGenerateException(O.Line, O.Column,
                                                                L"GetTypeOfAST: Cannot covert '" +
                                                                Environment.ClassPool[Left.OriginalType.ClassIndexInGlobalEnvironment].ClassName +
                                                                L"' to '" +
                                                                Environment.ClassPool[Right.OriginalType.ClassIndexInGlobalEnvironment].ClassName);
                            }
                        }
                        break;
                    }
                }
            } else {
                Lexer::Token O = Target.GetFirstNotNullToken();
                throw BytecodeGenerateException(O.Line, O.Column,
                                                L"GetTypeOfAST: Cannot covert function derive or array derive to any other types.");
            }
            Result = Right;
            break;
        }
        case AST::TreeType::Identifier: {
            auto Val = Environment.SearchSymbol(EnvIndex, Target.Node.Value);
            if (Val.second != -1) {
                Result = Environment.Environments[Val.first].SymbolItem[Val.second].Type;
            } else {
                throw BytecodeGenerateException(Target.Node.Line, Target.Node.Column,
                                                L"GetTypeOfAST: Undefined symbol : " + Target.Node.Value);
            }
            break;
        }
        case AST::TreeType::IndexExpression: {
            auto Val = GetTypeOfAST(Target.Subtrees[0]);
            if (Val.Kind != TypenameDerive::DeriveKind::ArrayDerive) {
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
        case AST::TreeType::BinaryExpression:
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
        case AST::TreeType::VariableDeclaration:
        case AST::TreeType::VariableDefinition:
        case AST::TreeType::CodeBlockStatement:
        case AST::TreeType::AssignmentExpression: {
            Result.Kind = TypenameDerive::DeriveKind::InvalidTypename;
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