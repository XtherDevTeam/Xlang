//
// Created by Jerry Chou on 2022/3/27.
//

#include "BytecodeGenerator.hpp"

#include <utility>

BytecodeGenerator::BytecodeGenerator() : Source(), EnvironmentStack() {

}

BytecodeGenerator::BytecodeGenerator(AST Source) : Source(std::move(Source)) {}

BytecodeCommandArray BytecodeGenerator::Generate(AST &Target) {
    switch (Target.Type) {
        case AST::TreeType::NotMatch:
            break;
        case AST::TreeType::Primary:
            break;
        case AST::TreeType::ExtendPrimary:
            break;
        case AST::TreeType::Identifier:
            break;
        case AST::TreeType::Arguments:
            break;
        case AST::TreeType::Operator:
            break;
        case AST::TreeType::IndexExpression:
            break;
        case AST::TreeType::FunctionCallingExpression:
            break;
        case AST::TreeType::MemberExpression:
            break;
        case AST::TreeType::NegativeExpression:
            break;
        case AST::TreeType::IncrementExpression:
            break;
        case AST::TreeType::DecrementExpression:
            break;
        case AST::TreeType::MultiplicationExpression:
            break;
        case AST::TreeType::AdditionExpression:
            break;
        case AST::TreeType::BinaryMoveExpression:
            break;
        case AST::TreeType::ComparingExpression:
            break;
        case AST::TreeType::EqualExpression:
            break;
        case AST::TreeType::BinaryExpression:
            break;
        case AST::TreeType::LogicExpression:
            break;
        case AST::TreeType::AssignmentExpression:
            break;
    }
}
