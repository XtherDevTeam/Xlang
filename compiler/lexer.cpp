#include <bits/stdc++.h>
#include "error.cpp"
using namespace std;
#define NTOKEN 1000
//定义词法单元值
enum TokenValue {
    TOK_COMMA,          //逗号
    TOK_DIV,
    TOK_END,
    TOK_EQUAL,TOK_2EQUAL,TOK_NOTEQUAL,TOK_MAXEQUAL,TOK_MINEQUAL,
    TOK_MAX,TOK_MIN,
    TOK_ID,
    TOK_IF,
    TOK_INTEGER,
    TOK_STRING,
    TOK_MINUS,TOK_2MINUS,
    TOK_MULT,
    TOK_PLUS,TOK_2PLUS,
    TOK_DOT,
    TOK_SEMICOLON,
    TOK_MBRACKETL,TOK_MBRACKETR, // ()
    TOK_CBRACKETL,TOK_CBRACKETR, // []
    TOK_BBRACKETL,TOK_BBRACKETR, // {}
    TOK_CHARTER,
};
string TOKEN_VALUE_DESCRIPTION[] =
{
    "TOK_COMMA",
    "TOK_DIV",
    "TOK_END",
    "TOK_EQUAL","TOK_2EQUAL","TOK_NOTEQUAL","TOK_MAXEQUAL","TOK_MINEQUAL",
    "TOK_MAX","TOK_MIN",
    "TOK_ID",
    "TOK_IF",
    "TOK_INTEGER",
    "TOK_STRING",
    "TOK_MINUS","TOK_2MINUS",
    "TOK_MULT",
    "TOK_PLUS","TOK_2PLUS",
    "TOK_DOT",
    "TOK_SEMICOLON",
    "TOK_MBRACKETL","TOK_MBRACKETR",
    "TOK_CBRACKETL","TOK_CBRACKETR",
    "TOK_BBRACKETL","TOK_BBRACKETR",
    "TOK_CHARTER",
};
//标签和标签的值
typedef class _Token {
    public:
    int  type;
    string str;
    _Token(){type = TOK_END;}
    _Token(int Type,string Str){type = Type;str=Str;}
    bool operator==(const _Token &other){
        if(type == other.type && str == other.str)  return true;
        return false;
    }
} Token;

class Lexer{
    public:
    string Text; // Put the codes here
    char *text_begin,*current,*text_end; // current char and text begin pointter,and text end pointer
    int position; // for get substr
    void Next(){
        if(current == text_end){
            return;
        }
        text_begin = &Text[0];
        text_end = &Text[Text.length()];
        current++;
        position++;
    }
    void Reset(){
        text_begin = &Text[0];
        text_end = &Text[Text.length()];
        current = text_begin;
        position = 0;
    }
    Lexer(string t){
        Text = t;
        text_begin = &Text[0];
        text_end = &Text[Text.length()];
        current = text_begin;
        position = 0;
    }
    bool IsExpression(){
        int flag1=0,flag2=0,flag3=0;
        for(int i = 0;i < Text.length();i++){
                 if(Text[i] == '(')  flag1++ ;else if(Text[i] == ')')  flag1--;
            else if(Text[i] == '[')  flag2++ ;else if(Text[i] == ']')  flag2--;
            else if(Text[i] == '{')  flag3++ ;else if(Text[i] == '}')  flag3--;
            if(flag1 == 0 && flag2 == 0 && flag3 == 0){
                if((i != Text.length() - 1 && Text[i] == '=' && Text[i+1] == '=')        ||
                    Text[i] == '+' || Text[i] == '-' || Text[i] == '*' || Text[i] == '/' ||
                    Text[i] == '%' || Text[i] == '<' || Text[i] == '>' || Text[i] == '!'  )
                {
                    return true;
                }
            }
            else if(flag1 < 0) throw ParserError("No match bracket find at "+to_string(i)+" : cannot match bracket '('\n");
            else if(flag2 < 0) throw ParserError("No match bracket find at "+to_string(i)+" : cannot match bracket '['\n");
            else if(flag3 < 0) throw ParserError("No match bracket find at "+to_string(i)+" : cannot match bracket '{'\n");
        }
        return false;
    }
    bool EndOfText(){
        if(*current == ' '){
            while(*current == ' '){Next();}
        }
        if(*current == '\0'){
            return true;
        }
        return false;
    }
    Lexer subLexer(int begin = INT_MAX,int length = INT_MAX){
        return Lexer(Text.substr((begin == INT_MAX) ? position : begin,(length == INT_MAX) ? Text.length() : length));
    }
    Token getNextToken(){
        if(*current == '\0'){
            return Token(TOK_END,"\0");
        }
        if(*current == ' '){
            while(*current == ' '){Next();}
        }
        if(*current == '"'){
            Next();
            int start = position;
            while(*current != '"'){if(*current == '\\'){Next();Next();}else{Next();}}
            int length = position - start;
            Next();
            return Token(TOK_STRING,Text.substr(start,length));
        }
        if(*current == '\''){
            Next();
            int start = position;
            while(*current != '\''){
                if(*current == '\\'){Next();Next();}
                else{
                    Next();
                }
            }
            int length = position - start;
            Next();
            return Token(TOK_CHARTER,Text.substr(start,length));
        }
        if(*current == ';'){Next();return Token(TOK_SEMICOLON,";");}
        if(*current == ','){Next();return Token(TOK_COMMA,",");}
        if(*current == '.'){Next();return Token(TOK_DOT,".");}
        if(*current == '+'){
            Next();
            if(*current == '+'){Next();return Token(TOK_2PLUS,"++");}
            else return Token(TOK_PLUS,"+");
        }
        if(*current == '-'){
            Next();
            if(*current == '-'){Next();return Token(TOK_2MINUS,"--");}
            else return Token(TOK_MINUS,"-");
        }
        if(*current == '*'){Next();return Token(TOK_MULT,"*");}
        if(*current == '/'){Next();return Token(TOK_DIV,"/");}
        if(*current == '='){
            Next();
            if(*current == '=') {Next();return Token(TOK_2EQUAL,"==");}
            else return Token(TOK_EQUAL,"=");
        }
        if(*current == '!'){ Next();if(*current == '=') {Next();return Token(TOK_NOTEQUAL,"!=");}else{throw ParserError("Undefined Token at" + to_string(position));} }
        if(*current == '<'){ Next();if(*current == '=') {Next();return Token(TOK_MAXEQUAL,"<=");}else{return Token(TOK_MAX,"<");} }
        if(*current == '>'){ Next();if(*current == '=') {Next();return Token(TOK_MINEQUAL,">=");}else{return Token(TOK_MIN,">");} }
        if(*current == '('){Next();return Token(TOK_MBRACKETL,"(");}if(*current == ')'){Next();return Token(TOK_MBRACKETR,")");}
        if(*current == '['){Next();return Token(TOK_CBRACKETL,"[");}if(*current == ']'){Next();return Token(TOK_CBRACKETR,"]");}
        if(*current == '{'){Next();return Token(TOK_BBRACKETL,"{");}if(*current == '}'){Next();return Token(TOK_BBRACKETR,"}");}
        if(isdigit(*current)){
            int begin = position;
            while(isdigit(*current)){Next();}
            int length = position - begin;
            //Next();
            return Token(TOK_INTEGER,Text.substr(begin,length));
        }
        if(isalpha(*current)){
            int begin = position;
            while(isalpha(*current)){Next();}
            int length = position - begin;
            //Next();
            return Token(TOK_ID,Text.substr(begin,length));
        }
        throw ParserError("Undefined Token at" + to_string(position));
    }
};

enum AST_nodeType{
    FunctionDeclaration,
    ClassDeclaration,
    IfStatement,
    ForStatement,
    FunctionCallStatement,
    VariableDeclaration,
    ExpressionStatement,
    BlockStatement,
    Args,
    Id,
    Unused,
};

string AST_nodeType[] = {
    "FunctionDeclaration",
    "ClassDeclaration",
    "IfStatement",
    "ForStatement",
    "FunctionCallStatement",
    "VariableDeclaration",
    "ExpressionStatement",
    "Args",
    "Id",
    "Unused"
};

struct RegisterStatus{
    bool IsUsed_This;
    bool IsUsed_High;
    bool IsUsed_Low;
};

class Symbol{
    public:
    string name;
    size_t alloc_addr;
    size_t alloc_size;
    Symbol(){}
    Symbol(string name,size_t allocadr,size_t size){ this->name = name;this->alloc_addr = allocadr;this->alloc_size = size; }
};

map<string,RegisterStatus> RegsStat;
map<string,Symbol> symbol_table; // Flat memory manager,to locate struct's subvar,just <struct offset>+<variable addr>
size_t symbol_top = 0;

class ASTree{
    public:
    int nodeT;
    Token this_node;
    vector<ASTree> node;
    public:
    ASTree(){nodeT = Unused;} // For STL Vector
    ASTree(int nodeT,Token tok){
        this->nodeT = nodeT,this_node = tok,this->node = node;
    };
    ASTree(Lexer &lexer){
        lexer.Reset(); // reset lexer to first token
        Token current_tok = lexer.getNextToken();
        if(lexer.IsExpression()){

        }
        if(current_tok.type == TOK_ID){
            if(current_tok.str == "int"){
                nodeT = VariableDeclaration;
                this_node = current_tok;
                node.push_back( ASTree( Id,( current_tok = lexer.getNextToken() ) ) );
                symbol_table[current_tok.str] = Symbol(current_tok.str,symbol_top,4);
                symbol_top+=4+1; // href to next top
                if(( current_tok = lexer.getNextToken() ).type==TOK_END)  return; // Nothing can script again
                else if(current_tok.type != TOK_EQUAL) throw ParserError("Processing AST: Invalid Int definition");
                auto templex = lexer.subLexer();
                node.push_back( ASTree( templex ) );
                return;
            }
            if(current_tok.str == "char"){
                nodeT = VariableDeclaration;
                this_node = current_tok;
                node.push_back( ASTree( Id,( current_tok = lexer.getNextToken() ) ) );
                symbol_table[current_tok.str] = Symbol(current_tok.str,symbol_top,1);
                symbol_top+=1+1; // href to next top
                if(( current_tok = lexer.getNextToken() ).type==TOK_END)  return; // Nothing can script again
                else if(current_tok.type != TOK_EQUAL) throw ParserError("Processing AST: Invalid Charter definition");
                auto templex = lexer.subLexer();
                node.push_back( ASTree( templex ) );
                return;
            }
            if(current_tok.str == "ptr"){
                nodeT = VariableDeclaration;
                this_node = current_tok;
                node.push_back( ASTree( Id,( current_tok = lexer.getNextToken() ) ) );
                if(current_tok.str != "int" || current_tok.str != "char")  throw ParserError("Processing AST: Invalid pointer definition");
                node.push_back( ASTree( Id,( current_tok = lexer.getNextToken() ) ) );
                symbol_table[current_tok.str] = Symbol(current_tok.str,symbol_top,1);
                symbol_top+=1+1; // href to next top
                if(( current_tok = lexer.getNextToken() ).type==TOK_END)  return; // Nothing can script again
                else if(current_tok.type != TOK_EQUAL) throw ParserError("Processing AST: Invalid pointer definition");
                auto templex = lexer.subLexer();
                node.push_back( ASTree( templex ) );
                return;
            }else{
                if(symbol_table[current_tok.str].name == "")  throw ParserError("Processing AST:Undefined symbol:" + current_tok.str);
                this_node = current_tok;
                if(lexer.getNextToken().type != TOK_END){
                    auto templex = lexer.subLexer();
                    node.push_back( ASTree( templex ) );
                }
                return;
            }
        }
        if(current_tok.type == TOK_INTEGER){
            if(lexer.getNextToken().type != TOK_END) throw ParserError("Processing AST: Interger doesn't any sub variable!");
            this->nodeT = Id;
            this_node = current_tok;
            return;
        }
        if(current_tok.type = TOK_CHARTER){
            if(lexer.getNextToken().type != TOK_END) throw ParserError("Processing AST: Charter doesn't any sub variable!");
            this->nodeT = Id;
            this_node = current_tok;
            return;
        }
    }
};

//int main(){}