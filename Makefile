# Makefile for Xlang compiler and Xtime VM
CXX := g++-10 # for my mac, you can set it to g++
CXXFLAGS = -std=c++2a -w -O2 -g
compiler: xlang xlang.cpp ./compiler/compiler.cpp
	$(CXX) $(CXXFLAGS) ./xlang.cpp -o xlang

vm: ./vm/vm ./vm/vm.cpp ./vm/core.cpp
	$(CXX) $(CXXFLAGS) ./vm/vm.cpp -o ./vm/vm

debug_release:
	$(CXX)  ./xlang.cpp -o xlang	-g -w
	$(CXX) ./vm/vm.cpp -o ./vm/vm	-g -w

all: compiler vm

run: all
	./xlang && ./vm/vm
