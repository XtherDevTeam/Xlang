#include "compiler/compiler.cpp"
using namespace std;

int main(){
    InitCompiler();
    while(!cin.eof()){
        try{
            string input;
            getline(cin,input);
            Lexer lexer(input);
            Token this_tok = lexer.getNextToken();
            while(this_tok.type != TOK_END){
                cout << "\033[32m[" << TOKEN_VALUE_DESCRIPTION[this_tok.type] << "]\033[0m: " << this_tok.str << "\t";
                this_tok = lexer.getNextToken();
            }
            cout << "IsExpression(): " << lexer.IsExpression() << endl;
            ASTree ast(lexer);
            ast.prettyPrint();
            //cout << "generated asmcode:\n" << ast.dumpToAsm() << endl;
            StartDebuger();
        }catch(ParserError &e){
            e.what();
        }
    }
}