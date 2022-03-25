cmake_minimum_required(VERSION 3.21)
project(Xlang2)

set(CMAKE_CXX_STANDARD 17)

add_executable(Xlang2
        main.cpp
        frontend/SourceFileAnalyzer.cpp
        share/config.hpp frontend/Lexer.cpp
        frontend/Lexer.hpp share/IOStream.cpp
        share/IOStream.hpp share/FileStream.cpp
        share/StringUtils.cpp share/XlangException.cpp
        )
