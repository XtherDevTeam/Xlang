CXX = g++
CXXFLAGS = -w -std=c++17 -g

test_lex: test_lex.cpp
	g++ ./test_lex.cpp -o test_lex

all: test_lex

