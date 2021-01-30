#include "compiler/compiler.cpp"
using namespace std;

int main(){
    InitCompiler();
    string s;
    getline(cin,s);
    Lexer lex(s);
    cout << dumpToAsm(ASTree(s)).toString() << endl;
}
