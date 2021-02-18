# Makefile for Xlang compiler and Xtime VM
CC := g++-10 # for my mac, you can set it to g++
CCFLAGS = -std=c++2a -w -O2 -g
compiler: ./xlang.cpp compiler/*.cpp
	$(CC) ./xlang.cpp -o xlang $(CCFLAGS)

vm: ./vm/vm.cpp ./vm/core.cpp
	$(CC) ./vm/vm.cpp -o ./vm/vm $(CCFLAGS)

all: compiler vm

run: all
	./xlang && ./vm/vm