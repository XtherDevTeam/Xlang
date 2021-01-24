#include "compiler/compiler.cpp"
using namespace std;

int main(){
    Lexer lex("ast.wdnm.print(\"Hello,World\",114514)");
    ASTree ast(lex);
    
}
