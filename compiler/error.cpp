#include <bits/stdc++.h>

class ParserError{
    std::string msg;
    public:
    ParserError(){}
    ParserError(std::string msg){this->msg = msg;}
    void what(){
        std::cerr << "\033[31m[ParserError]\033[0m:" << msg << std::endl;
    }
};