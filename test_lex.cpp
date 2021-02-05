#include "compiler/compiler.cpp"
 

int main(){
    InitCompiler();
    while(! std::cin.eof()){
        try{
            std::string input;
            getline( std::cin,input);
            Lexer lexer(input);
            Token this_tok = lexer.getNextToken();
            while(this_tok.type != TOK_END){
                std::cout << "\033[32m[" << TOKEN_VALUE_DESCRIPTION[this_tok.type] << "]\033[0m: " << this_tok.str << "\t";
                this_tok = lexer.getNextToken();
            }
            std::cout << "IsExpression(): " << lexer.IsExpression() <<  std::endl;
            ASTree ast(lexer);
            ast.prettyPrint();
            //std::cout << "generated asmcode:\n" << ast.dumpToAsm() <<  std::endl;
            StartDebuger();
        }catch(ParserError &e){
            e.what();
        }
    }
}