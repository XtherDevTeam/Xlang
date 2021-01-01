#include "compiler/lexer.cpp"
using namespace std;

void StartDebuger(){
    cout << "Press ^C to debug...\n";
    getchar();
}

int main(){
    while(!cin.eof()){
        string input;
        getline(cin,input);
        Lexer lexer(input);
        Token this_tok = lexer.getNextToken();
        while(this_tok.type != TOK_END){
            cout << "\033[32m[" << TOKEN_VALUE_DESCRIPTION[this_tok.type] << "]\033[0m: " << this_tok.str << endl;
            this_tok = lexer.getNextToken();
        }

    }
}