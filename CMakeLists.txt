cmake_minimum_required(VERSION 3.21)
project(Xlang2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-static")
set(CMAKE_C_FLAGS "-static")

add_executable(Xlang2
        main.cpp
        share/config.hpp frontend/Lexer.cpp
        frontend/Lexer.hpp share/IOStream.cpp
        share/IOStream.hpp share/FileStream.cpp
        share/StringUtils.cpp share/XlangException.cpp
        frontend/AST.cpp frontend/AST.hpp
        frontend/parsers/ParserException.hpp
        frontend/parsers/ParserException.cpp
        frontend/parsers/PrimaryNodeGenerator.cpp
        frontend/parsers/PrimaryNodeGenerator.hpp
        frontend/parsers/BaseGenerator.cpp
        frontend/parsers/BaseGenerator.hpp
        frontend/parsers/IndexExpressionNodeGenerator.cpp
        frontend/parsers/IndexExpressionNodeGenerator.hpp
        frontend/parsers/FunctionCallingExpressionNodeGenerator.cpp
        frontend/parsers/FunctionCallingExpressionNodeGenerator.hpp
        frontend/parsers/IdentifierNodeGenerator.cpp
        frontend/parsers/IdentifierNodeGenerator.hpp
        frontend/parsers/ArgumentsNodeGenerator.cpp
        frontend/parsers/ArgumentsNodeGenerator.hpp
        frontend/parsers/MemberExpressionNodeGenerator.cpp
        frontend/parsers/MemberExpressionNodeGenerator.hpp
        frontend/parsers/NegativeExpressionNodeGenerator.cpp
        frontend/parsers/NegativeExpressionNodeGenerator.hpp
        frontend/parsers/DecrementExpressionNodeGenerator.cpp
        frontend/parsers/DecrementExpressionNodeGenerator.hpp
        frontend/parsers/MultiplicationExpressionNodeGenerator.cpp
        frontend/parsers/MultiplicationExpressionNodeGenerator.hpp
        frontend/parsers/AdditionExpressionNodeGenerator.cpp
        frontend/parsers/AdditionExpressionNodeGenerator.hpp
        frontend/parsers/BinaryMoveExpressionNodeGenerator.cpp
        frontend/parsers/BinaryMoveExpressionNodeGenerator.hpp
        frontend/parsers/ComparingExpressionNodeGenerator.cpp
        frontend/parsers/ComparingExpressionNodeGenerator.hpp
        frontend/parsers/EqualExpressionNodeGenerator.cpp
        frontend/parsers/EqualExpressionNodeGenerator.hpp
        frontend/parsers/IncrementExpressionNodeGenerator.cpp
        frontend/parsers/IncrementExpressionNodeGenerator.hpp
        frontend/parsers/BinaryExpressionNodeGenerator.cpp
        frontend/parsers/BinaryExpressionNodeGenerator.hpp
        frontend/parsers/LogicExpressionNodeGenerator.cpp
        frontend/parsers/LogicExpressionNodeGenerator.hpp
        frontend/parsers/AssignmentExpressionNodeGenerator.cpp
        frontend/parsers/AssignmentExpressionNodeGenerator.hpp
        frontend/parsers/ExpressionNodeGenerator.cpp
        frontend/parsers/ExpressionNodeGenerator.hpp
        backend/BytecodeGenerateException.hpp
        backend/BytecodeGenerateException.cpp
        frontend/parsers/ParserException.hpp
        bytecode/Operand.hpp
        bytecode/Operand.cpp
        bytecode/BytecodeCommand.hpp
        bytecode/BytecodeCommand.cpp
        backend/LocalEnvironment.cpp
        backend/LocalEnvironment.hpp
        backend/Typename.cpp backend/Typename.hpp
        xtimevm/XlangClass.cpp
        xtimevm/XlangClass.hpp
        bytecode/BytecodeCommandArray.cpp
        bytecode/BytecodeCommandArray.hpp
        backend/BytecodeGenerator.cpp
        backend/BytecodeGenerator.hpp
        backend/GlobalEnvironment.cpp
        backend/GlobalEnvironment.hpp
        backend/XlangFunction.cpp
        backend/XlangFunction.hpp
        backend/TypenameDerive.cpp
        backend/TypenameDerive.hpp
        backend/XlangClass.cpp
        backend/XlangClass.hpp
        frontend/parsers/TypeSpecifierNodeGenerator.cpp
        frontend/parsers/TypeSpecifierNodeGenerator.hpp
        frontend/parsers/AccessDescriptorNodeGenerator.cpp
        frontend/parsers/AccessDescriptorNodeGenerator.hpp frontend/parsers/VariableDescriptorNodeGenerator.cpp frontend/parsers/VariableDescriptorNodeGenerator.hpp frontend/parsers/MethodDescriptorNodeGenerator.cpp frontend/parsers/MethodDescriptorNodeGenerator.hpp frontend/parsers/VariableDefinitionNodeGenerator.cpp frontend/parsers/VariableDefinitionNodeGenerator.hpp)
