#include "compiler/compiler.cpp"
 

int main(){
    InitCompiler();
    ConstPool_Apis::Init(cp,1024);
    std::string s;
    getline( std::cin,s);
    Lexer lex(s);
    try{
        std::cout << dumpToAsm(ASTree(s),1).tostd::string() <<  std::endl;
    }catch (CompileError e){
        e.what();
    }catch(ParserError e){
        e.what();
    }
}