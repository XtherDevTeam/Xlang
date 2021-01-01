#include <bits/stdc++.h>
using namespace std;
#define NTOKEN 1000
//定义词法单元值
enum TokenValue {
    TOK_COMMA,          //逗号
    TOK_DIV,
    TOK_END,
    TOK_EQUAL,
    TOK_ID,
    TOK_IF,
    TOK_INTEGER,
    TOK_MINUS,
    TOK_MULT,
    TOK_PLUS,
    TOK_SEMICOLON,
    TOK_UNDEFINED,
};
string TOKEN_VALUE_DESCRIPTION[] =
{
    "TOK_COMMA",
    "TOK_DIV",
    "TOK_END",
    "TOK_EQUAL",
    "TOK_ID",
    "TOK_IF",
    "TOK_INTEGER",
    "TOK_MINUS",
    "TOK_MULT",
    "TOK_PLUS",
    "TOK_SEMICOLON",
    "TOK_UNDEFINED",
};
//标签和标签的值
typedef class _Token {
    public:
    int  type;
    string str;
    _Token(int Type,string Str){type = Type;str=Str;}
} Token;

class Lexer{
    string Text; // Put the codes here
    char *text_begin,*current,*text_end; // current char and text begin pointter,and text end pointer
    int position; // for get substr
    public:
    void Next(){
        if(current == text_end){
            return;
        }
        text_begin = &Text[0];
        text_end = &Text[Text.length()];
        current++;
        position++;
    }
    Lexer(string t){
        Text = t;
        text_begin = &Text[0];
        text_end = &Text[Text.length()];
        current = text_begin;
        position = 0;
    }
    Token getNextToken(){
        if(*current == '\0'){
            return Token(TOK_END,"\0");
        }
        if(*current == ' '){
            while(*current == ' '){Next();}
        }
        if(*current == ';'){Next();return Token(TOK_SEMICOLON,";");}
        if(*current == ','){Next();return Token(TOK_COMMA,",");}
        if(*current == '+'){Next();return Token(TOK_PLUS,"+");}
        if(*current == '-'){Next();return Token(TOK_MINUS,"-");}
        if(*current == '*'){Next();return Token(TOK_MULT,"*");}
        if(*current == '/'){Next();return Token(TOK_DIV,"/");}
        if(*current == '='){Next();return Token(TOK_EQUAL,"=");}
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
    }
};

class AST{
    public:
    string this_node;
    vector<AST> nodes; // 通过检查nodes的size来确认是否为节点
};