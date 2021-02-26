# Makefile for Xlang compiler and Xtime VM
CXX := g++-10 # for my mac, you can set it to g++
CXXFLAGS = -std=c++2a -w -O2 -g -ldl

compiler: 
	$(CXX) xlang.cpp -o xlang $(CXXFLAGS)

vm: 
	$(CXX) vm/vm.cpp -o ./vm/vm $(CXXFLAGS)

debug_release:
	$(CXX)  ./xlang.cpp -o xlang	-g -w -ldl
	$(CXX) ./vm/vm.cpp -o ./vm/vm	-g -w -ldl

all: compiler vm

lib: lib/vmlib@latest/native
	cd lib/vmlib@latest/native && make all

run: all
	./xlang && ./vm/vm

clean:
	rm -rf test_lex.dSYM test_lex xlang.dSYM test.dSYM xlang test_lex
