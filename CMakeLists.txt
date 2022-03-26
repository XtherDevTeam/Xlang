cmake_minimum_required(VERSION 3.21)
project(Xlang2)

set(CMAKE_CXX_STANDARD 17)

add_executable(Xlang2
        main.cpp
        share/config.hpp frontend/Lexer.cpp
        frontend/Lexer.hpp share/IOStream.cpp
        share/IOStream.hpp share/FileStream.cpp
        share/StringUtils.cpp share/XlangException.cpp
        frontend/AST.cpp frontend/AST.hpp
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
        frontend/parsers/ParserException.cpp frontend/parsers/ParserException.hpp)
