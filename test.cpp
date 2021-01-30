#include "compiler/compiler.cpp"
using namespace std;

int main(){
    InitCompiler();
    ConstPool_Apis::Init(cp,1024);
    string s;
    getline(cin,s);
    Lexer lex(s);
    cout << dumpToAsm(ASTree(s),1).toString() << endl;
}