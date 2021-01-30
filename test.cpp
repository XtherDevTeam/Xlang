#include "compiler/compiler.cpp"
using namespace std;

int main(){
    InitCompiler();
    ConstPool_Apis::Init(cp,1024);
    string s;
    getline(cin,s);
    Lexer lex(s);
    try{
        cout << dumpToAsm(ASTree(s),1).toString() << endl;
    }catch (CompileError e){
        e.what();
    }catch(ParserError e){
        e.what();
    }
}