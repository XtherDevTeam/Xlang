#include <bits/stdc++.h>
#include "error.cpp"
#include "basic_type.cpp"
// 
#define NTOKEN 1000

void StartDebuger(){
    std::cout << "Press ^C to debug...";
    getchar();
}

//定义词法单元值
enum TokenValue {
    TOK_COMMA,          //逗号
    TOK_DIV,
    TOK_END,
    TOK_EQUAL,TOK_2EQUAL,TOK_NOTEQUAL,TOK_MINEQUAL,TOK_MAXEQUAL,
    TOK_MAX,TOK_MIN,
    TOK_ID,
    TOK_IF,
    TOK_INTEGER,
    TOK_DOUBLE,
    TOK_STRING,
    TOK_MINUS,TOK_2MINUS,
    TOK_MULT,
    TOK_PLUS,TOK_2PLUS,
    TOK_DOT,
    TOK_COLON,
    TOK_SEMICOLON,
    TOK_ARGSTATEMENT,
    TOK_CBRACKETL,TOK_CBRACKETR, // []
    TOK_BBRACKET,
    TOK_MBRACKET,
    TOK_BLOCK, // Block statement
    TOK_CHARTER,
};
std::string TOKEN_VALUE_DESCRIPTION[] =
{
    "TOK_COMMA",
    "TOK_DIV",
    "TOK_END",
    "TOK_EQUAL","TOK_2EQUAL","TOK_NOTEQUAL","TOK_MINEQUAL","TOK_MAXEQUAL",
    "TOK_MAX","TOK_MIN",
    "TOK_ID",
    "TOK_IF",
    "TOK_INTEGER",
    "TOK_DOUBLE",
    "TOK_STRING",
    "TOK_MINUS","TOK_2MINUS",
    "TOK_MULT",
    "TOK_PLUS","TOK_2PLUS",
    "TOK_DOT",
    "TOK_COLON",
    "TOK_SEMICOLON",
    "TOK_ARGSTATEMENT",
    "TOK_CBRACKETL","TOK_CBRACKETR",
    "TOK_BBRACKET",
    "TOK_MBRACKET",
    "TOK_BLOCK",
    "TOK_CHARTER",
};
//标签和标签的值
typedef class _Token {
    public:
    int  type;
    std::string str;
    _Token(){type = TOK_END;}
    _Token(int Type,std::string Str){type = Type;str=Str;}
    bool operator==(const _Token &other){
        if(type == other.type && str == other.str)  return true;
        return false;
    }
} Token;

int getOperatorLevel(Token tok){
    // 计算符等级由高到低
    if(tok.type == TOK_EQUAL || tok.type == TOK_COLON) return 3;
    if(tok.type == TOK_2EQUAL || tok.type == TOK_MINEQUAL || tok.type == TOK_MAXEQUAL || tok.type == TOK_NOTEQUAL || tok.type == TOK_MAX || tok.type == TOK_MIN)  return 2;
    if(tok.type == TOK_PLUS || tok.type == TOK_MINUS) return 1;
    if(tok.type == TOK_MULT || tok.type == TOK_DIV)  return 0;
    if(tok.type == TOK_DOT) return -1;
    else return INT_MAX;
}
class Lexer{
    public:
    std::string Text; // Put the codes here
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
    Lexer(std::string t){
        Text = t;
        int realstart=0;
        if(Text[0] == ' '){
            for(realstart=0;realstart < Text.length();realstart++){
                if(Text[realstart] != ' ') break;
            }
        }

        Text = Text.substr(realstart);
        text_begin = &Text[0];
        text_end = &Text[Text.length()];
        current = text_begin;
        position = 0;
    }
    bool IsExpression(){
        int flag1=0,flag2=0,flag3=0,iscontent = 0;
        bool starflag = 0,isexpr = 0;
        int i = 0;
        if(Text[i] == ' '){
            while(Text[i] == ' ') i++;
        }
        for(;i < Text.length();i++){
                 if(Text[i] == '(')  flag1++ ;else if(Text[i] == ')')  flag1--;
            else if(Text[i] == '[')  flag2++ ;else if(Text[i] == ']')  flag2--;
            else if(Text[i] == '{')  flag3++ ;else if(Text[i] == '}')  flag3--;
            else if(Text[i] == '"') iscontent = !iscontent;
            //std::cout << "\033[30m" << Text[i] << iscontent << flag1 << flag2 << flag3 << ((Text[i] == ' ' || Text[i] == ',') && iscontent == 0 && flag3 == 0 && flag1 == 0 && flag2 == 0) << "\033[0m" <<  std::endl;
            if(((Text[i] == ' ' && Text[i-1] <= 'z' && Text[i-1] >= 'A' && Text[i+1] <= 'z' && Text[i+1] >= 'A') || Text[i] == ',') && iscontent == 0 && flag3 == 0 && flag1 == 0 && flag2 == 0){
                return false;
            }
            if(flag1 == 0 && flag2 == 0 && flag3 == 0 && !iscontent){
                if( (i != Text.length() - 1 && Text[i] == '=' && Text[i+1] == '=')       ||
                    Text[i] == '+' || Text[i] == '-' || Text[i] == '*' || Text[i] == '/' ||
                    Text[i] == '%' || Text[i] == '<' || Text[i] == '>' || Text[i] == '!' || Text[i] == '=' || ( Text[i] == '.' && (Text[i-1] < '0' && Text[i-1] > '9') == false) || Text[i] == ':')
                {
                    isexpr = 1;
                }
            }
            else if(flag1 < 0) throw ParserError("No match bracket find at "+std::to_string(i)+" : cannot match bracket '('\n");
            else if(flag2 < 0) throw ParserError("No match bracket find at "+std::to_string(i)+" : cannot match bracket '['\n");
            else if(flag3 < 0) throw ParserError("No match bracket find at "+std::to_string(i)+" : cannot match bracket '{'\n");
        }
        return isexpr;
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
            while(*current != '\''){if(*current == '\\'){Next();Next();}else{Next();}}
            int length = position - start;
            Next();
            return Token(TOK_CHARTER,Text.substr(start,length));
        }
        if(*current == ';'){Next();return Token(TOK_SEMICOLON,";");}
        if(*current == ','){Next();return Token(TOK_COMMA,",");}
        if(*current == '.'){Next();return Token(TOK_DOT,".");}
        if(*current == '+'){Next();if(*current == '+'){Next();return Token(TOK_2PLUS,"++");}else{return Token(TOK_PLUS,"+");}}
        if(*current == '-'){Next();if(*current == '-'){Next();return Token(TOK_2MINUS,"--");}else{return Token(TOK_MINUS,"-");}}
        if(*current == '*'){Next();return Token(TOK_MULT,"*");}
        if(*current == ':'){Next();return Token(TOK_COLON,":");}
        if(*current == '/'){Next();return Token(TOK_DIV,"/");}
        if(*current == '='){ Next();if(*current == '=') {Next();return Token(TOK_2EQUAL,"==");}else{return Token(TOK_EQUAL,"=");} }
        if(*current == '{'){
            Next();
            int start = position;
            int count = 0;
            while(true){
                if(*current == '{'){ count++; }if(*current == '}'){ count--; if(count == -1){break;} }if(*current == '\0'){throw ParserError("TokenError: Cannot find '}' in this text");}if(*current == '\\'){Next();Next();}Next();
            }
            int length = position - start;
            Next();
            return Token(TOK_BLOCK,Text.substr(start,length));
        }
        if(*current == '('){
            Next();
            int start = position;
            int count = 0;
            while(true){
                if(*current == '('){ count++; }
                if(*current == ')'){ count--; if(count == -1){break;} }
                if(*current == '\0')  throw ParserError("TokenError: Cannot find ')' in this text");
                if(*current == '\\'){Next();Next();}
                Next();
            }
            int length = position - start;
            Next();
            return Token(TOK_ARGSTATEMENT,Text.substr(start,length));
        }
        if(*current == '!'){ Next();if(*current == '=') {Next();return Token(TOK_NOTEQUAL,"!=");}else{throw ParserError("Undefined Token at" + std::to_string(position));} }
        if(*current == '<'){ Next();if(*current == '=') {Next();return Token(TOK_MINEQUAL,"<=");}else{return Token(TOK_MIN,"<");} }
        if(*current == '>'){ Next();if(*current == '=') {Next();return Token(TOK_MAXEQUAL,">=");}else{return Token(TOK_MAX,">");} }
        if(*current == '['){Next();return Token(TOK_CBRACKETL,"[");}if(*current == ']'){Next();return Token(TOK_CBRACKETR,"]");}
        if(isdigit(*current)){
            TokenValue ret = TOK_INTEGER;
            int begin = position;
            while(isdigit(*current) || *current == '.'){if(*current == '.'){ret = TOK_DOUBLE;}Next();}
            int length = position - begin;
            //Next();
            return Token(ret,Text.substr(begin,length));
        }
        if(isalpha(*current) || *current == '_'){
            int begin = position;
            while(isalpha(*current)|| *current == '_'){Next();}
            int length = position - begin;
            //Next();
            return Token(TOK_ID,Text.substr(begin,length));
        }
        throw ParserError("Undefined Token at" + std::to_string(position));
    }
    bool EndOfText(){
        if(*(current+1) == '\0' || *(current) == '\0'){return true;}
        return false;
    }
};

enum AST_nodeType{
    ExpressionStatement,
    BlockStatement,
    FunctionCallStatement,
    NormalStatement,
    Args,
    Id,
    Unused,
};

std::string AST_nodeType[] = {
    "ExpressionStatement",
    "BlockStatement",
    "FunctionCallStatement",
    "NormalStatement",
    "Args",
    "Id",
    "Unused"
};

struct RegisterStatus{
    bool IsUsed_This;
    bool IsUsed_High;
    bool IsUsed_Low;
};
std::string emptyStr(int size){
    std::string s="";
    s.resize(size);
    for(int i = 0;i < size;i++){s[i] = ' ';}
    return s;
}

class ASTree{
    public:
    int nodeT;
    Token this_node;
    std::vector<ASTree> node;
    public:
    ASTree(){nodeT = Unused;} // For STL std::vector
    ASTree(int nodet,Token tok){
        nodeT = nodet;
        this_node = tok;
    };
    void prettyPrint(int swap=1){
        std::cout << "{\n" << emptyStr(swap);
        printf("Type: \"%s\",\n" , AST_nodeType[nodeT].c_str());
        std::cout << emptyStr(swap);
        printf(
            "{ Type: \"%s\" , Content: \"%s\" },\n",
            TOKEN_VALUE_DESCRIPTION[this_node.type].c_str(),
            this_node.str.c_str()
        );
        std::cout << emptyStr(swap) << "[" << ((node.size() <= 1) ? ' ' : '\n');
        for(auto i=0;i<node.size();i++){
            std::cout << emptyStr(swap+1);
            node[i].prettyPrint(swap+2);
            std::cout << emptyStr(swap+1);
        }
        std::cout << "\b" << emptyStr(swap-1) << "],\n" << emptyStr(swap-1) << "},\n";
    }
    ASTree(Lexer lexer){
        lexer.Reset(); // reset lexer to first token
        Token current_tok = lexer.getNextToken();
        if(lexer.IsExpression()){
            lexer.Reset();
            //TODO: ADD EXPRESSION AST GENRATEOR
            int sb = 0; // 哨兵一，记录上一个token的位置
            Token lastEvalToken;int lastEvalPosL = 0,lastEvalPosR = 0;char lock_status;
            for(Token tok = lexer.getNextToken();tok.type != TOK_END;tok = lexer.getNextToken()){
                //std::cout << tok.str << (getOperatorLevel(tok) != INT_MAX ) << ( ( getOperatorLevel(tok) > getOperatorLevel(lastEvalToken) || getOperatorLevel(lastEvalToken) == INT_MAX)) <<  std::endl;
                if(lock_status){lock_status = 0;lastEvalPosR = sb;}
                if(getOperatorLevel(tok) != INT_MAX && ( getOperatorLevel(tok) >= getOperatorLevel(lastEvalToken) || getOperatorLevel(lastEvalToken) == INT_MAX)){lastEvalToken = tok;lastEvalPosL = sb;lock_status = true;}
                sb = lexer.position;
            }
            // 遍历完成，获得最高级的左数和右树
            Lexer LeftTokenList (lexer.Text.substr(0,lastEvalPosL));
            Lexer RightTokenList(lexer.Text.substr(lastEvalPosR));
            this_node = lastEvalToken;
            nodeT = ExpressionStatement;
            node.push_back( ASTree(LeftTokenList) );
            node.push_back( ASTree(RightTokenList) );
            return;
        }
        if(current_tok.type == TOK_BLOCK){
            int count1=0,count2=0,count3=0,instr = 0; // (),[],{} don't find ','
            std::string temp_str = current_tok.str,current_str = "";
            for (size_t i = 0; i < temp_str.length(); i++){
                if(temp_str[i] == '(')  count1++;
                else if(temp_str[i] == '[')  count2++;
                else if(temp_str[i] == '{')  count3++;
                else if(temp_str[i] == ')')  count1--;
                else if(temp_str[i] == ']')  count2--;
                else if(temp_str[i] == '}')  count3--;
                else if(temp_str[i] == '"')  instr = !instr;
                else if(temp_str[i] == ';' && count1 == 0 && count2 == 0 && count3 == 0 && instr == 0){
                    Lexer temp_lexer(current_str);
                    node.push_back( ASTree(temp_lexer) );
                    current_str.clear();
                    continue;
                }
                current_str += temp_str[i];
            }
            nodeT=BlockStatement;
            this_node = Token(TOK_BBRACKET ,"{}");
            if(current_str == "") return;
            Lexer temp_lexer(current_str);
            node.push_back( ASTree(temp_lexer) );
            return;
        }
        if(current_tok.type == TOK_ARGSTATEMENT){
            int count1=0,count2=0,count3=0,instr = 0; // (),[],{} don't find ','
            std::string temp_str = current_tok.str,current_str = "";
            for (size_t i = 0; i < temp_str.length(); i++){
                if(temp_str[i] == '(')  count1++;
                else if(temp_str[i] == '[')  count2++;
                else if(temp_str[i] == '{')  count3++;
                else if(temp_str[i] == ')')  count1--;
                else if(temp_str[i] == ']')  count2--;
                else if(temp_str[i] == '}')  count3--;
                else if(temp_str[i] == '"')  instr = !instr;
                else if(temp_str[i] == ',' && count1 == 0 && count2 == 0 && count3 == 0 && instr == 0){
                    Lexer temp_lexer(current_str);
                    node.push_back( ASTree(temp_lexer) );
                    current_str.clear();
                    continue;
                }
                current_str += temp_str[i];
            }
            nodeT=Args;
            this_node = Token(TOK_MBRACKET,"()");
            if(current_str != ""){
                Lexer temp_lexer(current_str);
                node.push_back( ASTree(temp_lexer) );
            }
            return;
        }
        if(current_tok.type == TOK_ID){
            if(lexer.EndOfText()){
                //std::cout << lexer.Text << lexer.position <<  std::endl;
                this_node = current_tok;
                this->nodeT = Id;
                return;
            }
            this->nodeT = NormalStatement;
            this_node = current_tok;
            int sb = lexer.position,lastTokPosition = lexer.position;
            if(Lexer(lexer.Text.substr(sb)).IsExpression()){
                node.push_back(ASTree(lexer.subLexer(sb)));
                return;
            }
            for (auto tok = lexer.getNextToken(); tok.type != TOK_END; tok = lexer.getNextToken()){
                //std::cout << "\033[30m" << TOKEN_VALUE_DESCRIPTION[tok.type] << "\033[0m";
                if(tok.type == TOK_COMMA){
                    Lexer templex( lexer.Text.substr(sb,lastTokPosition - sb) );
                    node.push_back( ASTree(templex) );
                    sb = lexer.position;
                }
                if(tok.type == TOK_ARGSTATEMENT){
                    std::string s = lexer.Text.substr(sb,lastTokPosition - sb);
                    if(s != ""){Lexer templex( s );node.push_back( ASTree(templex) );}
                    Lexer templex( "("+tok.str+")" );
                    node.push_back( ASTree(templex) );
                    sb = lexer.position;
                }
                if(tok.type == TOK_BLOCK){
                    /*std::cout << "皇帝的新tok.str >>>" << tok.str << (tok.str == "") <<  std::endl;
                    if(tok.str == ""){
                        sb = lexer.position;
                        continue;
                    }*/
                    std::string s = lexer.Text.substr(sb,lastTokPosition - sb);
                    if(s != ""){Lexer templex( s );node.push_back( ASTree(templex) );}
                    Lexer templex( "{"+tok.str+"}" );
                    node.push_back( ASTree(templex) );
                    sb = lexer.position;
                }
                lastTokPosition = lexer.position;    
            }
            if(node.size() == 1 && node.empty() == false && node[0].nodeT == Args)  this->nodeT = FunctionCallStatement;
            if(lexer.Text.substr(sb) != ""){
                Lexer last( lexer.Text.substr(sb) );
                node.push_back( ASTree(last) );
            }
            return;
        }
        if(current_tok.type == TOK_INTEGER || current_tok.type == TOK_CHARTER || current_tok.type == TOK_STRING){
            if(lexer.getNextToken().type != TOK_END){
                std::cout << lexer.Text <<  std::endl;
                throw ParserError("Processing AST: Constant doesn't any sub script!");
            }
            nodeT = Id;
            this_node = current_tok;
            return;
        }
        this->nodeT=Unused;
    }
};

namespace ASTree_APIs{
    namespace MemberExpression{
        bool hasFunctionCallStatement(ASTree ast){
            if(ast.node.empty()) return false;
            if(ast.nodeT == FunctionCallStatement) return true;
            if(ast.nodeT == ExpressionStatement && ast.this_node.type == TOK_DOT){
                if(hasFunctionCallStatement(ast.node[1])) return true;
            }
            return false;
        }
    };
};