#include "core.cpp"
#include "../vm/core.cpp"
#define __DONT_CATCH_ERROR 

std::string& replace_all(std::string& str,const std::string& old_value,const std::string& new_value)     
{     
    while(true){     
        std::string::size_type   pos(0);     
        if((pos=str.find(old_value))!=std::string::npos){
            str.replace(pos,old_value.length(),new_value);     
        }
        else break;     
    }     
    return str;     
} 

std::string processEscape(std::string& origin){
    replace_all(origin,"\\0","\0");
    replace_all(origin,"\\\\","__qwertyuiopasdfghjklzxcvbnm_asdfghjkl_");
    replace_all(origin,"\\n","\n");
    replace_all(origin,"\\r","\r");
    replace_all(origin,"\\t","\t");
    replace_all(origin,"\\033","\033");
    replace_all(origin,"\\b","\b");
    replace_all(origin,"__qwertyuiopasdfghjklzxcvbnm_asdfghjkl_","\\\\");
    return origin;
}

struct ASM_Command{
    std::string Main;
    std::vector<std::string> args;
    std::string tostring(){
        std::stringstream ss;
        ss << Main << " ";
        for(int i = 0;i < args.size();i++){
            ss << args[i] << ",";
        }
        std::string ret = ss.str();
        ret.erase(ret.length() - 1);
        ret += ";\n";
        return ret;
    }
};

class ASMBlock{
    ASM_Command temp;
    public:
    std::string name;
    std::vector<ASM_Command> lists;
    ASMBlock(){}
    void operator+=(ASMBlock o){
        if(o.lists.empty()) return; 
        for(int i = 0;i < o.lists.size();i++){
            lists.push_back(o.lists[i]);
        }
    }
    ASM_Command& operator[](int index){return lists[index];}
    ASMBlock& genCommand(std::string s){
        if(temp.Main != ""){
            lists.push_back(temp);
            temp.Main = "";
            temp.args.clear();
        }
        temp.Main = s;
        return *this;
    }
    ASMBlock& genArg(std::string s){
        if(temp.Main == ""){throw ParserError("You must make a command beforce add arg.");}
        temp.args.push_back(s);
        return *this;
    }
    ASMBlock& push(){
        lists.push_back(temp);
        temp.Main = "";
        temp.args.clear();
        return *this;
    }
    std::string tostring(){
        std::stringstream ss;
        ss << name << ":" <<  std::endl;
        for(int i = 0;i < lists.size();i++){
            ss << lists[i].tostring();
        }
        return ss.str();
    }
};

std::string guessType(ASTree ast);

enum dumpToAsmState{
    CaseMode,
    GlobalMode,
    LoopMode,
};

class CompileError{
    std::string msg;
    public:
    CompileError(std::string msg){this->msg = msg;}
    void what(){
        std::cerr << "\033[31m[CompileError]\033[0m There was an error has been throwed in compile process:\n" << msg << "\033[0m\n";
    }
};

enum VarType{
    __BASIC_1BYTE,
    __BASIC_8BYTE,
    __OBJECT
};

class TypeName{
    public:
    std::string  name;
    VarType type;
    int size;
    std::map<std::string,TypeName> objects;

    TypeName& operator[](std::string s){
        if(objects.find(s) == objects.end()){
            // there may has a bomb in future.BUT!I DON'T CARE IT!!!!
            throw CompileError("Undefined Id:" + s);
        }
        return objects[s];
    }
    void operator+=(TypeName& o){
        if(type != __OBJECT)  throw CompileError("Cannot extends from an normally variable:" + name);
        name = o.name;
        size += o.size;
        for(auto i = o.objects.begin();i != o.objects.end();i++){
            objects[i->first] = i->second;
        }
    }
    TypeName(){
        this->size = 0;
    }
    TypeName(std::string name,VarType type){
        this->name = name;
        this->type = type;
        if(type == __BASIC_8BYTE) this->size = 8;
        else if(type == __BASIC_1BYTE) this->size = 1;
        else this->size = 0;
    }
    void InsertToObject(std::string s,TypeName t){
        size += t.size;
        if(objects.find(s) != objects.end()) throw CompileError("Already Exist: " + s);
        objects[s] = t;
    }
    size_t getOffset(ASTree ast,size_t originoffset = 0){
        if(ast.nodeT != ExpressionStatement || ast.this_node.str != "."){
            if(ast.nodeT == Id){
                return originoffset;
            }
            throw ParserError("Not an member expression statement");
        }else{
            return operator[](ast.node[0].this_node.str).getOffset(ast.node[1],originoffset + operator[](ast.node[0].this_node.str).size);
        }
    }
    TypeName& findObject(ASTree ast){
        if(ast.nodeT == Id) return operator[](ast.this_node.str);
        if(objects.find(ast.node[0].this_node.str) == objects.end()) throw CompileError("Object doesn't have member names " + ast.node[0].this_node.str);
        return operator[](ast.node[0].this_node.str).findObject(ast.node[1]);
    }
};

std::map<std::string,TypeName> type_pool;

/*
 * 关于结构体内函数定义的说明
 * 假设有一个结构体:
 * struct test{
 *  func int fuckcpp(int:a){
 *    return 1;
 *  }
 * }
 * 那函数在全局符号表内将会显示为:_@test_fuckcpp
 */

addr_t sp,cpsp;

enum SymbolType{
    Variable,
    Function,
    Object,
};

class Symbol{
    public:
    //SymbolType type;
    std::string _Typename;
    size_t frame_position;
    Symbol(std::string typename_){
        frame_position = sp;
        _Typename = typename_;
        if(type_pool.find(_Typename) == type_pool.end()) throw CompileError("Undefined Typename as " + _Typename);
        sp += type_pool[_Typename].size;
    }
    Symbol(){}
};

typedef class Function_{
    public:
    std::string funcname;
    TypeName type;
    Function_(){}
    Function_(std::string s,TypeName t){
        funcname = s;
        type = t;
    }
    std::string getRealname(){
        return (funcname[0] == '_' ? funcname : "_" + funcname) + "_" + type.name;
    }
} function_definition;

class function_block{
    public:
    std::string from_struct; // WARN:警告，默认from_struct为空
    addr_t restore_symboltop;
    ASTree type_and_args;
    ASTree block_statement;
    std::map<std::string,Symbol> syminfunc;
    function_block(ASTree args,ASTree bls,std::string from_str){
        this->from_struct = from_str;
        restore_symboltop = 0;
        addr_t tsp = sp;sp = 0;
        if(from_str != "") sp += 8; // 如果是从结构体创建的函数，则保留一个this指针
        type_and_args = args;
        block_statement = bls;
        if(type_and_args.node.empty()) return; // no args here
        for(int i = 0;i < type_and_args.node.size();i++){
            syminfunc[type_and_args.node[i].node[1].this_node.str] = Symbol(type_and_args.node[i].node[0].this_node.str);
        }
        restore_symboltop = sp;
        sp = tsp;
    }
    function_block(){
        restore_symboltop = 0;
    }
};

std::map<std::string,Symbol> global_symbol_table; // 存放在常量池里的全局符号表
std::map<std::string,Symbol> symbol_table;
std::map<std::string,function_block> function_table;

std::string getFunctionRealName(ASTree a,TypeName& this_scope){
    if(!ASTree_APIs::MemberExpression::hasFunctionCallStatement(a))  return ""; // It's a stupid fix.But,It's work now.
    else{
        if(a.node[1].nodeT == FunctionCallStatement) return "_@" + this_scope.findObject(a.node[0]).name  + "_" + a.node[1].this_node.str;
        else{
            return getFunctionRealName(a.node[1],this_scope[a.node[0].this_node.str]);
        }
    }
}


std::string getFunctionRealName(ASTree a){
    if(a.nodeT == FunctionCallStatement) return "_"+a.this_node.str;
    if(a.nodeT != ExpressionStatement) return "";
    if(type_pool.count(guessType(a.node[0])) == 0) return "";
    if(a.node[1].nodeT == FunctionCallStatement){
        return "_@" + guessType(a.node[0]) + "_" + a.node[1].this_node.str;
    }
    return getFunctionRealName(a.node[1],type_pool[guessType(a.node[0])]);
}

ASTree getFunctionCallArgs(ASTree ast){
    if(ast.nodeT == FunctionCallStatement) return ast.node[0];
    else if(ast.nodeT == ExpressionStatement && ast.this_node.type == TOK_DOT) return getFunctionCallArgs(ast.node[1]);
}

std::string funcnameInTab(std::string realname){
    for(auto i = function_table.begin();i != function_table.end();i++){
        if(i->first.substr(0,realname.length()) == realname) return i->first;
    }
    throw CompileError("Cannot find current overload in function table");
}

ConstantPool cp;

namespace ConstPool_Apis
{
    void Init(ConstantPool& cpool,size_t size = 1024){
        cpool.pool = (char*)realloc(cpool.pool,size);
        cpool.items = (size_t*)malloc(1024);
        memset(cpool.items,0,1024);
    }
    addr_t Insert(ConstantPool& cpool,char* _Src,addr_t size){
        memcpy(cpool.pool + cpool.items[cpool.count],_Src,size);
        if(cpool.count == 0){
            cpool.items[cpool.count+1] = size;
            cpool.size = size;
            return cpool.count++;
        }
        cpool.count++;
        cpool.items[cpool.count] = cpool.items[cpool.count-1] + size;
        cpool.size += size;
        return cpool.count-1;
    }
} // namespace ConstPool_Apis

bool isNormalExpression(ASTree ast){
    if(ast.this_node.type != TOK_DOT && ast.this_node.type != Id && ast.this_node.type != TOK_ID && ast.this_node.type != TOK_STRING) return true;
    return false;
}

std::string guessType(ASTree ast){
    if(ast.nodeT == Id){
        if((ast.this_node.type == TOK_ID || ast.this_node.type == TOK_PTRID) && symbol_table.count(ast.this_node.str))  return symbol_table[ast.this_node.str]._Typename;
        else if((ast.this_node.type == TOK_ID || ast.this_node.type == TOK_PTRID) && global_symbol_table.count(ast.this_node.str)) return global_symbol_table[ast.this_node.str]._Typename;
        switch (ast.this_node.type)
        {
        case TOK_STRING:
            return "ptr_char";
        
        case TOK_INTEGER:
            return "int";
        
        case TOK_DOUBLE:
            return "int";
        
        case TOK_CHARTER:
            return "char";
        
        default:
            throw CompileError("Unknown Constant:" + TOKEN_VALUE_DESCRIPTION[ast.this_node.type]);
            break;
        }
    }else if(ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast)){
        return funcnameInTab(getFunctionRealName(ast)).substr(getFunctionRealName(ast).length()+1);
    }else if(ast.nodeT == ExpressionStatement){
        if(ast.this_node.type == TOK_DOT){
            if(ast.node[0].this_node.type == TOK_ID){
                if(symbol_table.count(ast.node[0].this_node.str)) return type_pool[symbol_table[ast.node[0].this_node.str]._Typename].findObject(ast.node[1]).name;
                else if(global_symbol_table.count(ast.node[0].this_node.str)) return type_pool[global_symbol_table[ast.node[0].this_node.str]._Typename].findObject(ast.node[1]).name;
                else throw CompileError(ast.node[0].this_node.str + " doesn't exist");   
            }else if(ast.node[0].this_node.type == TOK_PTRID){
                std::string result = (global_symbol_table.count(ast.node[0].this_node.str)) ? \
                                    type_pool[global_symbol_table[ast.node[0].this_node.str]._Typename.substr(4)].findObject(ast.node[1]).name : \
                                    type_pool[symbol_table[ast.node[0].this_node.str]._Typename.substr(4)].findObject(ast.node[1]).name;
                return result;
            }else if(ast.node[0].this_node.type == TOK_PTRB){
                std::string fullname = guessType(ASTree(Lexer(ast.node[0].this_node.str))); // 猜这个指针是什么类型的
                std::string result = (global_symbol_table.count(fullname)) ? \
                                    type_pool[global_symbol_table[fullname]._Typename.substr(4)].findObject(ast.node[1]).name : \
                                    type_pool[symbol_table[fullname]._Typename.substr(4)].findObject(ast.node[1]).name;
                return result;
            }
        }else if(ast.this_node.type == TOK_COLON){
            return ast.node[0].this_node.str; // 送 业 绩
        }else{
            if(guessType(ast.node[0]) == "ptr_char" || guessType(ast.node[1]) == "ptr_char") return "ptr_char";
            return guessType(ast.node[0]);
        }
    }else if(ast.this_node.type == TOK_PTRB){
        return guessType(ast.node[0].node[0]);
    }
}

addr_t getMemberSize(ASTree ast){
    return type_pool[guessType(ast)].size;
}

void InitCompiler(){
    ConstPool_Apis::Init(cp);
    type_pool["int"]    = TypeName("int",__BASIC_8BYTE);
    type_pool["char"]   = TypeName("char",__BASIC_1BYTE);
    type_pool["double"] = TypeName("double",__BASIC_8BYTE);
    type_pool["ptr_int"]= TypeName("ptr_int",__BASIC_8BYTE);
    type_pool["ptr_char"]= TypeName("ptr_char",__BASIC_8BYTE);
    type_pool["ptr_double"]= TypeName("ptr_double",__BASIC_8BYTE);
}

bool RegState[32];
int getLastUsingRegId(){
    for(int i = 0;i < 30;i++){
        if(RegState[i] == false) return i;
    }
    return INT_MAX;
}

VarType getMemberType(ASTree ast,TypeName& scope){
    if(ast.nodeT == Id) return scope[ast.this_node.str].type;
    else if(ast.this_node.type == TOK_DOT) return getMemberType(ast.node[1],scope[ast.node[1].node[0].this_node.str]);
    else return type_pool[guessType(ast)].type;
}
VarType getMemberType(ASTree ast){
    if(ast.nodeT == Id && global_symbol_table.count(ast.this_node.str)) return type_pool[global_symbol_table[ast.this_node.str]._Typename].type;
    else if(symbol_table.count(ast.this_node.str)) return type_pool[symbol_table[ast.this_node.str]._Typename].type;
    return getMemberType(ast,type_pool[ast.node[0].this_node.str]);
}

ASMBlock dumpToAsm(ASTree ast,int mode = false/*default is cast mode(0),but in global,it's global mode(1)*/){
    if(ast.nodeT == Unused) return ASMBlock();
    if(ast.nodeT == Id){
        if(ast.this_node.type == TOK_INTEGER || ast.this_node.type == TOK_DOUBLE){
            if(ast.this_node.type == TOK_INTEGER) return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg(ast.this_node.str).push();
            else{
                Content s;
                s.dblc = atof(ast.this_node.str.c_str());
                return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg(std::to_string(s.intc)).push();
            }
        }
        if(ast.this_node.type == TOK_STRING){
            //std::cout << "???" << ast.this_node.str.size() << std::endl;
            processEscape(ast.this_node.str);
            return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg(std::to_string(ConstPool_Apis::Insert(cp,(char*)ast.this_node.str.c_str(),ast.this_node.str.length()))).push();
        }
        if(ast.this_node.type == TOK_CHARTER){
            std::cout << "beforce:   "  << ast.this_node.str << std::endl;
            processEscape(ast.this_node.str);
            std::cout << "processed: " << ast.this_node.str << std::endl;
            return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg(std::to_string((int)ast.this_node.str[0])).genCommand("push1b").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("reg" + std::to_string(getLastUsingRegId())).push();
        }
        if(symbol_table.find(ast.this_node.str) != symbol_table.end()){
            if(ast.this_node.type != TOK_PTRID || (ast.this_node.type == TOK_DOT && ast.node[0].this_node.type != TOK_PTRID)) return ASMBlock().genCommand("mov").genArg("reg"+std::to_string(getLastUsingRegId())).genArg("regsb").genCommand("sub").genArg("reg"+std::to_string(getLastUsingRegId())).genArg("regfp").genCommand("sub").genArg("reg" + std::to_string(getLastUsingRegId())).genArg(std::to_string(symbol_table[ast.this_node.str].frame_position + getMemberSize(ast) - 1)).push();
            else return ASMBlock().genCommand("mov").genArg("reg"+std::to_string(getLastUsingRegId()+1)).genArg("regsb").genCommand("sub").genArg("reg"+std::to_string(getLastUsingRegId()+1)).genArg("regfp").genCommand("sub").genArg("reg" + std::to_string(getLastUsingRegId()+1)).genArg(std::to_string(symbol_table[ast.this_node.str].frame_position + getMemberSize(ast) - 1)).genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("[reg" + std::to_string(getLastUsingRegId() + 1) + "]").push();
        }
        if(global_symbol_table.find(ast.this_node.str) != global_symbol_table.end()){
            if(ast.this_node.type != TOK_PTRID || (ast.this_node.type == TOK_DOT && ast.node[0].this_node.type != TOK_PTRID)) return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg(std::to_string(global_symbol_table[ast.this_node.str].frame_position)).push();
            else return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId()+1)).genArg(std::to_string(global_symbol_table[ast.this_node.str].frame_position)).genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("[reg" + std::to_string(getLastUsingRegId() + 1) + "]").push();
        }
        if(ast.this_node.str == "true")  return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("1").push();
        if(ast.this_node.str == "false")  return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("0").push();
    }
    if(ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast)){
        std::string func_name = getFunctionRealName(ast);
        ASTree args = getFunctionCallArgs(ast);
        ASMBlock asb;
        asb.genCommand("save").push();
        if(ast.this_node.type == TOK_DOT){
            ASTree fullname(Lexer(ASTree_APIs::MemberExpression::getFunctionPath(ast)));
            asb += dumpToAsm(fullname); // 结构体传址
            asb.genCommand("push").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("8");
        }
        if(args.nodeT == Args){
            ASTree type_a_names = function_table[funcnameInTab(func_name)].type_and_args;
            if((ast.this_node.type != TOK_DOT && args.node.size() != type_a_names.node.size()) || (ast.this_node.type == TOK_DOT && args.node.size()+1 != type_a_names.node.size())) throw CompileError("Too few/much args have been gave.");
            for(int i = 0;i < args.node.size();i++){ 
                asb += dumpToAsm(args.node[i]);
                std::string realarg0 = "reg" + std::to_string(getLastUsingRegId());
                //std::cout << "BUGHERE:" << AST_nodeType[ast.node[i].node[0].nodeT] <<  std::endl;
                if(args.node[i].this_node.type == TOK_INTEGER || args.node[i].this_node.type == TOK_DOUBLE || args.node[i].this_node.type == TOK_CHARTER || args.node[i].this_node.type == TOK_STRING || (args.node[i].nodeT == ExpressionStatement && args.node[i].this_node.type != TOK_DOT)) /*do nothing*/;
                else realarg0 = "[" + realarg0 + "]";
                asb.genCommand("push").genArg(realarg0).genArg(std::to_string(getMemberSize(args.node[i]))).push();
            }
        }
        //std::cout << "\033[32mGuess Result:\033[0m" << guessType(ast) << " " << getFunctionRealName(ast) <<  std::endl;
        asb.genCommand("call").genArg(funcnameInTab(func_name)).\
        genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("regsb").\
        genCommand("sub").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("regfp").\
        genCommand("sub").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("regsp").\
        genCommand("add").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("1").push();
        //genCommand("add").genArg("reg" + std::to_string(getLastUsingRegId())).genArg(std::to_string(getMemberSize(ast)));  //低端序，直接从下面读到上面
        sp += getMemberSize(ast);
        return asb;
    }
    if(ast.nodeT == Args && ast.this_node.type == TOK_PTRB){
        ASMBlock asb;
        asb += dumpToAsm(ast.node[0]);
        return asb.genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("[reg" + std::to_string(getLastUsingRegId()) + "]").push(); // 由于先算出_dest和_Src的地址，所以这种语法是完全允许的
    }
    if(ast.nodeT == ExpressionStatement){
        if(ast.this_node.type == TOK_PLUS || ast.this_node.type == TOK_MINUS || ast.this_node.type == TOK_MULT || ast.this_node.type == TOK_DIV || ast.this_node.type == TOK_2EQUAL || ast.this_node.type == TOK_NOTEQUAL || ast.this_node.type == TOK_MAXEQUAL || ast.this_node.type == TOK_MINEQUAL || ast.this_node.type == TOK_MAX || ast.this_node.type == TOK_MIN){
            //HIGHLIGHT: if(ast.this_node.type == TOK_NOTEQUAL){std::cout << "eof";sleep(5);}
            ASMBlock ab;
            ab += dumpToAsm(ast.node[0],mode);
            if(ast.node[0].this_node.type == TOK_PTRID || ast.node[0].this_node.type == TOK_PTRB) ab.genCommand("push1b").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("[reg" + std::to_string(getLastUsingRegId()) + "]").push();
            //if(ast.node[0].nodeT == FunctionCallStatement || ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[0])) ab.genCommand("").genArg("reg" + std::to_string(getLastUsingRegId()));
            RegState[getLastUsingRegId()] = true;
            ab += dumpToAsm(ast.node[1],mode);
            if(ast.node[1].this_node.type == TOK_PTRID || ast.node[1].this_node.type == TOK_PTRB) ab.genCommand("push1b").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("[reg" + std::to_string(getLastUsingRegId()) + "]").push();
            if(ast.node[1].nodeT == FunctionCallStatement || ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[1])) sp -= getMemberSize(ast.node[1]);
            RegState[getLastUsingRegId()] = true;
            if(ast.this_node.type == TOK_PLUS) ab.genCommand("add");
            else if(ast.this_node.type == TOK_MINUS) ab.genCommand("sub");
            else if(ast.this_node.type == TOK_MULT) ab.genCommand("mul");
            else if(ast.this_node.type == TOK_DIV) ab.genCommand("div");
            else if(ast.this_node.type == TOK_2EQUAL) ab.genCommand("equ");
            else if(ast.this_node.type == TOK_MAXEQUAL) ab.genCommand("maxeq");
            else if(ast.this_node.type == TOK_MINEQUAL) ab.genCommand("mineq");
            else if(ast.this_node.type == TOK_MAX) ab.genCommand("max");
            else if(ast.this_node.type == TOK_MIN) ab.genCommand("min");
            else if(ast.this_node.type == TOK_NOTEQUAL) ab.genCommand("neq");
            else if(ast.this_node.type == TOK_2EQUAL) ab.genCommand("equ");
            // Xlang变量的dumpASM会传地址
            // 加减乘除默认8byte运算
            //if(ast.node[0].this_node.type == TOK_PTRID || ast.node[0].this_node.type == TOK_PTRB) ab.genArg("[reg" + std::to_string(getLastUsingRegId() - 2) + "]");
            if(isNormalExpression(ast.node[0])) ab.genArg("reg" + std::to_string(getLastUsingRegId() - 2));
            else if(ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[0])){
                ab.genArg("[reg" + std::to_string(getLastUsingRegId() - 2) + "]");
                sp -= getMemberSize(ast.node[0]);
            }
            else if(getMemberType(ast.node[0]) == __BASIC_8BYTE || getMemberType(ast.node[0]) == __BASIC_1BYTE ) ab.genArg("[reg" + std::to_string(getLastUsingRegId() - 2) + "]");
            else throw CompileError("TypeError: 加减乘除以及逻辑运算仅限于基础类型,除非你想让虚拟机崩掉.\nBasic operator and boolean expression only support basic types,if you want to let the vm crash then don't do it.");
            //if(ast.node[1].this_node.type == TOK_PTRID || ast.node[1].this_node.type == TOK_PTRB) ab.genArg("[reg" + std::to_string(getLastUsingRegId() - 2) + "]");
            if(isNormalExpression(ast.node[1])) ab.genArg("reg" + std::to_string(getLastUsingRegId() - 1));
            else if(ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[1])) ab.genArg("[reg" + std::to_string(getLastUsingRegId() - 1) + "]");
            else if(getMemberType(ast.node[1]) == __BASIC_8BYTE || getMemberType(ast.node[1]) == __BASIC_1BYTE || ast.node[1].this_node.type == TOK_PTRID || ast.node[1].this_node.type == TOK_PTRB) ab.genArg("[reg" + std::to_string(getLastUsingRegId() - 1) + "]");
            else throw CompileError("TypeError: 加减乘除以及逻辑运算仅限于基础类型,除非你想让虚拟机崩掉.\nBasic operator and boolean expression only support basic types,if you want to let the vm crash then don't do it.");
            RegState[getLastUsingRegId() - 1] = false;
            RegState[getLastUsingRegId() - 1] = false;
            return ab.push();
        }
        if(ast.this_node.type == TOK_DOT){
            // address only
            int fp_offset;
            if(symbol_table.count(ast.node[0].this_node.str)) fp_offset = type_pool[symbol_table[ast.node[0].this_node.str]._Typename].getOffset(ast.node[1],symbol_table[ast.node[0].this_node.str].frame_position);
            else if(global_symbol_table.count(ast.node[0].this_node.str)) fp_offset = type_pool[global_symbol_table[ast.node[0].this_node.str]._Typename].getOffset(ast.node[1],global_symbol_table[ast.node[0].this_node.str].frame_position);
            if(symbol_table.count(ast.node[0].this_node.str)) return ASMBlock().genCommand("mov").genArg("reg"+std::to_string(getLastUsingRegId())).genArg("regsb").genCommand("sub").genArg("reg" + std::to_string(getLastUsingRegId())).genArg("regfp").genCommand("sub").genArg("reg" + getLastUsingRegId()).genArg(std::to_string(fp_offset + getMemberSize(ast) - 1)).push(); // 低端序
            else if(global_symbol_table.count(ast.node[0].this_node.str)) return ASMBlock().genCommand("mov").genArg("reg" + std::to_string(getLastUsingRegId())).genArg(std::to_string(fp_offset)).push();
            else throw CompileError("Variable " + ast.node[0].this_node.str + "doesn't exist");
        }
        if(ast.this_node.type == TOK_EQUAL){
            ASMBlock asb;
            asb += dumpToAsm(ast.node[0]); 
            RegState[getLastUsingRegId()] = true;
            asb += dumpToAsm(ast.node[1]);
            RegState[getLastUsingRegId() - 1] = false;
            std::string realarg0 = "reg" + std::to_string(getLastUsingRegId()+1);
            //std::cout << "BUGHERE:" << AST_nodeType[ast.node[i].node[0].nodeT] <<  std::endl;
            if(ast.node[1].this_node.type == TOK_INTEGER || ast.node[1].this_node.type == TOK_DOUBLE || ast.node[1].this_node.type == TOK_CHARTER || ast.node[1].this_node.type == TOK_STRING || (isNormalExpression(ast.node[1]) == false && ast.node[1].this_node.type != TOK_PTRID && ast.node[1].this_node.type != TOK_PTRB)) /*do nothing*/;
            else realarg0 = "[" + realarg0 + "]";
            asb.genCommand("mov_m").genArg("[reg" + std::to_string(getLastUsingRegId()) + "]").genArg(realarg0).genArg(std::to_string(getMemberSize(ast.node[1]))).push();
            return asb;
        }
    }
    if(ast.nodeT == BlockStatement){
        ASMBlock asb;
        if(ast.node.empty()) return ASMBlock();
        for(int i = 0;i < ast.node.size();i++){
            if(ast.node[i].nodeT == Unused) break;
            asb += dumpToAsm(ast.node[i]);
        }
        return asb;
    }
    if(ast.nodeT == NormalStatement){
        if(ast.this_node.str == "struct"){
            std::string struct_name = ast.node[0].this_node.str;
            TypeName t(struct_name,__OBJECT);
            ASTree& contents = ast.node[1];
            type_pool["ptr_" + struct_name] = TypeName("ptr_"+struct_name,__BASIC_8BYTE);
            type_pool["ptr_" + struct_name].size = 8;
            for(int i = 0;i < contents.node.size();i++){
                if(contents.node[i].this_node.str == "func"){
                    // TODO: add function definition processing core
                    std::string real_funcname = "_@" + struct_name + "_" + contents.node[i].node[0].node[0].this_node.str;
                    //if(contents.node[i].node[2].nodeT != BlockStatement) throw CompileError("func definition statement must have a codeblock!");
                    //function_table[function_definition(real_funcname,type_pool[contents.node[i].node[0].this_node.str]).getRealname()] = function_block(contents.node[i].node[1],contents.node[i].node[2]);
                    function_definition fdef(real_funcname,type_pool[contents.node[i].node[0].this_node.str]);
                    if(contents.node[i].node[2].nodeT != BlockStatement) throw CompileError("Function Definition Statemet must have an block statement");
                    function_table[fdef.getRealname()] = function_block(contents.node[i].node[1],contents.node[i].node[2],struct_name);
                    continue;
                }
                if(type_pool.count(contents.node[i].this_node.str) == 0) throw CompileError(contents.node[i].this_node.str + " doesn't an exist typename.");
                TypeName this_type = type_pool[contents.node[i].this_node.str];
                ASTree& temp_ast = contents.node[i];
                for(int j = 0;j < temp_ast.node.size();j++){
                    if(temp_ast.node[j].nodeT == Id){
                        t.InsertToObject(temp_ast.node[j].this_node.str,this_type);
                        
                    }
                    else throw CompileError("Compiler doesn't support init value now"); // TODO: Add init value support
                }
            }
            type_pool[struct_name] = t;
            return ASMBlock();
        }
        if(ast.this_node.str == "func"){
            if(mode != true) throw CompileError("Cannot create a function in cast mode.");
            std::string real_funcname = ast.node[0].node[0].this_node.str;
            function_definition fdef(real_funcname,type_pool[ast.node[0].this_node.str]);
            if(ast.node[2].nodeT != BlockStatement) throw CompileError("Function Definition Statemet must have an block statement");
            function_table[fdef.getRealname()] = function_block(ast.node[1],ast.node[2],"");
            return ASMBlock();
        }
        if(type_pool.count(ast.this_node.str)){
            TypeName& typen = type_pool[ast.this_node.str];
            ASMBlock asb;
            for(int i = 0;i < ast.node.size();i++){
                if(ast.node[i].nodeT == Id){
                    if(mode){
                        int cp_adr = ConstPool_Apis::Insert(cp,(char*)malloc(typen.size),typen.size); // 解释，这里是把全局变量放进常量池，初始化
                        global_symbol_table[ast.node[i].this_node.str].frame_position = cp.items[cp_adr]; // WARN: 挖坑
                        global_symbol_table[ast.node[i].this_node.str]._Typename = typen.name;
                        asb.genCommand("mov_m").genArg("[" + std::to_string(cp.items[cp_adr]) + "]").genArg(std::to_string(0)).genArg(std::to_string(typen.size)); // 防止内存泄漏，删除了这段代码，不对变量进行初始化
                        continue;
                    }
                    symbol_table[ast.node[i].this_node.str] = Symbol(typen.name);
                    asb.genCommand("sub").genArg("regsp").genArg(std::to_string(typen.size)); // stack由上往下
                }
                if(ast.node[i].nodeT == ExpressionStatement && ast.node[i].this_node.type == TOK_EQUAL){
                    // has init value
                    asb += dumpToAsm(ast.node[i].node[1]);
                    if(mode){
                        std::string realarg0 = "reg" + std::to_string(getLastUsingRegId());
                        //std::cout << "BUGHERE:" << AST_nodeType[ast.node[i].node[0].nodeT] <<  std::endl;
                        if(ast.node[i].node[1].this_node.type == TOK_INTEGER || ast.node[i].node[1].this_node.type == TOK_DOUBLE || ast.node[i].node[1].this_node.type == TOK_CHARTER || ast.node[i].node[1].this_node.type == TOK_STRING || (ast.node[i].node[1].nodeT == ExpressionStatement && ast.node[i].node[1].this_node.type != TOK_DOT)) /*do nothing*/;
                        else realarg0 = "[" + realarg0 + "]";
                        global_symbol_table[ast.node[i].node[0].this_node.str].frame_position = cp.items[cp.count];
                        global_symbol_table[ast.node[i].node[0].this_node.str]._Typename = typen.name;
                        //std::cout << "BUGHERE:" << typen.name <<  std::endl;
                        ConstPool_Apis::Insert(cp,(char*)malloc(typen.size),typen.size);
                        asb.genCommand("mov_m").genArg("[" + std::to_string(cp.items[cp.count - 1]) + "]");
                        asb.genArg(realarg0);
                        asb.genArg(std::to_string(typen.size));
                        continue;
                    }
                    std::string realarg0 = "reg" + std::to_string(getLastUsingRegId());
                    if(ast.node[i].node[1].this_node.type  == TOK_INTEGER || ast.node[i].node[1].this_node.type  == TOK_DOUBLE || ast.node[i].node[1].this_node.type  == TOK_CHARTER || ast.node[i].node[1].this_node.type == TOK_STRING || (ast.node[i].node[1].nodeT == ExpressionStatement && ast.node[i].node[1].this_node.type != TOK_DOT)) /* do nothing*/;
                    else realarg0 = "[" + realarg0 + "]";
                    symbol_table[ast.node[i].node[0].this_node.str] = Symbol(typen.name);
                    asb.genCommand("push").genArg(realarg0).genArg(std::to_string(typen.size));
                    //sp += typen.size;
                }
            }
            return asb.push();
        }
        if(ast.this_node.str == "if"){
            // regflag
            ASMBlock asb;
            if(ast.node[0].nodeT != Args || ast.node[1].nodeT != BlockStatement)  throw ParserError("SyntaxError: Bad If Statement");
            ASMBlock tmpblock = dumpToAsm(ast.node[1],mode); // end_pos - now_pos = jmp command
            asb += dumpToAsm(ast.node[0].node[0],mode);
            asb.genCommand("jt").genArg("2").genCommand("jf").genArg(std::to_string(tmpblock.lists.size()+1)).push();
            asb += tmpblock;
            asb.genCommand("_$fakecommand_goto_statement_end").push();
            if(ast.node.size() > 2) {
            for(int i = 2;i < ast.node.size();i+=3){
                if(ast.node[i].this_node.str == "elif"){
                    ASMBlock tmpblock = dumpToAsm(ast.node[i+2],mode);
                    tmpblock.genCommand("_$fakecommand_goto_statement_end").push(); // end_pos - now_pos = jmp command
                    asb += dumpToAsm(ast.node[i+1].node[0],mode);
                    asb.genCommand("jt").genArg("2").genCommand("jf").genArg(std::to_string(tmpblock.lists.size()+1)).push();
                    asb += tmpblock;
                }else if(ast.node[i].this_node.str == "else"){
                    asb += dumpToAsm(ast.node[i+1],mode);
                    continue;
                }else{
                    throw CompileError("Unknown Command:" + ast.node[i].this_node.str);
                }
            }
            }
            for(int i = 0;i < asb.lists.size();i++){
                if(asb.lists[i].Main == "_$fakecommand_goto_statement_end"){
                    asb.lists[i].Main = "jmp";
                    asb.lists[i].args.push_back(std::to_string( asb.lists.size() - 1 - i + 1 ));
                }
            }
            return asb;
        }
        if(ast.this_node.str == "for"){
            /*
            for(int i=0,0 == 0,i++)
            */
            /*
            mov 0,reg0;
            mov_m [0],reg0,8;

            mov 0,reg0;
            mov 0,reg1;
            equ reg0,reg1;
            jt 2;
            jf 6; // 4 = blocksize(2) + endofblocksize(3) + 1

            add [8],1;
            jmp 1; // blocksize - itself, 2 - 1 = 1

            add reg0,1;
            add reg1,1;
            jmp -7; // i - 1 - inital_val(2)
            */
            ASMBlock asb,inital_val,boolean_expression,for_blockstmt,endofblock;
            if(ast.node[0].nodeT != Args || ast.node[1].nodeT != BlockStatement)  throw ParserError("SyntaxError: Bad For Statement");

            inital_val = dumpToAsm(ast.node[0].node[0],mode);

            boolean_expression = dumpToAsm(ast.node[0].node[1],mode);

            for_blockstmt = dumpToAsm(ast.node[1],mode);

            endofblock = dumpToAsm(ast.node[0].node[2],mode);
            endofblock.genCommand("_$fakecommand_goto_loop_start").push();

            boolean_expression.genCommand("jt").genArg("2").genCommand("jf").genArg(std::to_string(for_blockstmt.lists.size() + endofblock.lists.size() + 1)).push();

            asb += inital_val;
            asb += boolean_expression;
            asb += for_blockstmt;
            asb += endofblock;

            for(int i = 0;i < asb.lists.size();i++){
                if(asb.lists[i].Main == "_$fakecommand_goto_loop_start"){
                    asb.lists[i].Main = "jmp";
                    asb.lists[i].args.push_back("-" + std::to_string( (int)(i - inital_val.lists.size()) ));
                }
                if(asb.lists[i].Main == "_$fakecommand_loop_continue"){
                    asb.lists[i].Main = "jmp";
                    asb.lists[i].args.push_back(std::to_string( asb.lists.size() - endofblock.lists.size() - i + 1));
                }
                if(asb.lists[i].Main == "_$fakecommand_goto_for_end"){
                    asb.lists[i].Main = "jmp";
                    //std::cout << "\033[31mwtf>>" << asb.lists.size() - i <<  std::endl;
                    asb.lists[i].args.push_back(std::to_string( asb.lists.size() - i + 1 ));
                }
            }
            return asb;
        }
        if(ast.this_node.str == "while"){
            if(ast.node[0].nodeT != Args || ast.node[1].nodeT != BlockStatement) throw CompileError("Bad While Statement");
            ASMBlock asb;
            asb += dumpToAsm(ast.node[0].node[0]);
            asb.genCommand("jt").genArg("2").genCommand("jf").genArg(std::to_string(dumpToAsm(ast.node[1]).lists.size() + 1 + 1)).push(); // command here
            asb += dumpToAsm(ast.node[1]).genCommand("_$fakecommand_loop_continue").push();
            for(long i = 0;i < asb.lists.size();i++){
                if(asb.lists[i].Main == "_$fakecommand_goto_for_end"){
                    asb.lists[i].Main = "jmp";
                    asb.lists[i].args.push_back(std::to_string(asb.lists.size() - i));
                }
                if(asb.lists[i].Main == "_$fakecommand_loop_continue"){
                    asb.lists[i].Main = "jmp";
                    asb.lists[i].args.push_back(std::to_string(0 - i));
                }
            }
            return asb;
        }
        if(ast.this_node.str == "break"){
            return ASMBlock().genCommand("_$fakecommand_goto_for_end").push();
        }
        if(ast.this_node.str == "continue"){
            return ASMBlock().genCommand("_$fakecommand_loop_continue").push();
        }
        if(ast.this_node.str == "return"){
            ASMBlock asb;
            asb += dumpToAsm(ast.node[0]);
            std::string realarg0 = "reg" + std::to_string(getLastUsingRegId());
            if(ast.node[0].this_node.type == TOK_INTEGER || ast.node[0].this_node.type == TOK_DOUBLE || ast.node[0].this_node.type == TOK_CHARTER || ast.node[0].this_node.type == TOK_STRING || (ast.node[0].nodeT == ExpressionStatement && ast.node[0].this_node.type != TOK_DOT)) /*do nothing*/;
            else realarg0 = "[" + realarg0 + "]";
            return asb.genCommand("ret").genArg(realarg0).genArg(std::to_string(getMemberSize(ast.node[0])))/*.genCommand("push").genArg(realarg0).genArg(std::to_string(getMemberSize(ast.node[0])))*/.push();
        }
        if(ast.this_node.str == "asm"){
            if(ast.node.size() != 1 || ast.node[0].this_node.type != TOK_STRING) throw CompileError("ASM Statement must be a asm string");
            Lexer lex(ast.node[0].this_node.str);
            ASMBlock asb;
            asb.genCommand(lex.getNextToken().str);
            for(auto tok = lex.getNextToken();tok.type != TOK_END;tok = lex.getNextToken()){
                if(tok.type == TOK_COMMA) continue;
                if(tok.type == TOK_CBRACKETL){
                    std::string tmp = tok.str;
                    while((tok = lex.getNextToken()).type != TOK_CBRACKETR && tok.type != TOK_END) tmp += tok.str;
                    asb.genArg(tmp += tok.str);
                    continue;
                }
                asb.genArg(tok.str);
            }
            return asb.push();
        }
        throw CompileError("Unknown Command: " + ast.this_node.str);
    }
    if(ast.nodeT == BlockStatement){
        ASMBlock ret;
        if(ast.node.empty()) return ret;
        for(int i = 0;i < ast.node.size();i++) ret += dumpToAsm(ast.node[i]);
        return ret;
    }
}

std::vector<ASMBlock> CompileProcess(std::string code){
    #ifndef __DONT_CATCH_ERROR
    try{
    #endif
    std::vector<ASMBlock> asblst;
    ASMBlock asb;
    int intext=0,block=0,brack1=0,brack2=0,brack3=0,descriptor = 0,bdescriptor = 0;
    std::string tmp;
    for(int i = 0;i < code.length();i++){
        if(code[i] == '/' && code[i+1] == '*' && !intext) bdescriptor = 1;
        if(code[i] == '*' && code[i+1] == '/' && !intext){
            bdescriptor = 0;i++;continue;
        }
        else if((code[i] == '\n' || code[i] == '\r') && !intext){
            descriptor = 0;
            if(code[i+1] == '/' && code[i+2] == '/' && bdescriptor == false) descriptor = 1;
            continue;
        }
        else if(code[i] == '\"') intext = !intext;
        else if(code[i] == '(') brack1++; else if(code[i] == ')') brack1--;
        else if(code[i] == '[') brack2++; else if(code[i] == ']') brack2--;
        else if(code[i] == '{') brack3++; else if(code[i] == '}') brack3--;
        //std::cout << (descriptor == 0) << (bdescriptor == 0) << std::endl;
        if(code[i] == ';' && brack1 == 0 && brack2 == 0 && brack3 == 0 && !intext && descriptor == 0 && bdescriptor == 0) {
            //std::cout << "HERE:" << tmp[0];
            Lexer lex(tmp);
            ASTree ast = ASTree(lex);
            if(ast.nodeT == FunctionCallStatement && ast.this_node.str == "import"){
                for(int i=0;i < ast.node[0].node.size();i++){
                    if(ast.node[0].node[i].node[0].this_node.str == "headers"){
                        std::string code;
                        if(ast.node[0].node[i].node[1].this_node.type != TOK_STRING) throw CompileError("SyntaxError: import(type:filepath,...)");
                        std::ifstream infile(ast.node[0].node[i].node[1].this_node.str);
                        while(!infile.eof()){
                            std::string tmpline;
                            getline(infile,tmpline);
                            code += tmpline + "\n";
                        }
                        std::vector<ASMBlock> list = CompileProcess(code);
                        for(int i = 0;i < list.size();i++){
                            if(list[i].name == "_vmstart"){asb += list[i];asb.lists.erase(--asb.lists.end());}
                            //else asblst.push_back(list[i]);
                        }
                    }else throw CompileError("SyntaxError: Unknown Import Kind:" + ast.node[0].node[i].node[0].this_node.str);
                }
                tmp = "";
                continue;
            }
            asb += dumpToAsm(ast,true);
            tmp = "";
            continue;
        }
        if(descriptor == false && bdescriptor == false) tmp += code[i];
    }
    // 默认起始点为main函数
    asb.genCommand("exit").push();
    asb.name = "_vmstart";
    asblst.push_back(asb);
    for(auto i = function_table.begin();i != function_table.end();i++){
        asb = ASMBlock();
        symbol_table = i->second.syminfunc;
        sp = i->second.restore_symboltop;
        asb = dumpToAsm(i->second.block_statement);
        asb.name = i->first;
        asblst.push_back( asb );
        sp = 0;
        symbol_table.clear();
    }
    return asblst;
    #ifndef __DONT_CATCH_ERROR
    }
    catch(CompileError e){
        e.what();
    }
    catch(ParserError e){
        e.what();
    }
    #endif
}

namespace Bytecode{
    enum opid_list{
        UnusualRegister = 1,
        NormalRegister  = 2,
        Command         = 3,
        Number          = 4,
        Address         = 5,
        Address_Register= 6,
    } opid_kind;
    std::string COMMAND_MAP[] = {
        "mov","mov_m","push","pop","save","pop_frame",
        "add","sub","mul","div",
        "equ","neq","maxeq","mineq","max","min",
        "jmp","jt","jf","call",
        "exit","ret","in","out","req","push1b","restore","fork"
    };
    int getCommandId(std::string command){
        for(int i = 0;i < 28;i=i+1){
            if(command == COMMAND_MAP[i]) return i;
        }
        std::cout << "Unknown Command Id:" << command << std::endl;
        return INT_MAX;
    }
    std::vector<ASMBlock> togen;
    CodeLabel *codelbls;
    int bytecode_top = 0;
    std::vector<ByteCode> bytecode;
    void Init(std::vector<ASMBlock> &blocks){
        codelbls = (CodeLabel*)malloc(blocks.size()*sizeof(CodeLabel));
        togen = blocks;
    }
    void Clear(){
        togen.clear();
        free(codelbls);
        bytecode.clear();
    }
    void exportBytecode(){
        for(int i = 0;i < togen.size();i++){
            codelbls[i].label_id = i;
            memset(codelbls[i].label_n,0,sizeof(codelbls[i].label_n));
            strcpy(codelbls[i].label_n,togen[i].name.c_str());
            codelbls[i].start = bytecode_top;
            for(int _each_command = 0;_each_command < togen[i].lists.size();_each_command++){
                if(togen[i][_each_command].Main == "") continue;
                ByteCode b;
                b.opid = Command;
                b.c.intc = getCommandId(togen[i][_each_command].Main);
                bytecode.push_back(b);
                bytecode_top++;
                for(int _each_arg = 0;_each_arg < togen[i][_each_command].args.size();_each_arg++){
                    ByteCode arg;
                    if(togen[i][_each_command].args[_each_arg].substr(0,3) == "reg"){
                        if(togen[i][_each_command].args[_each_arg].substr(3) == "fp" || togen[i][_each_command].args[_each_arg].substr(3) == "sp" || togen[i][_each_command].args[_each_arg].substr(3) == "pc" || togen[i][_each_command].args[_each_arg].substr(3) == "sb"){
                            arg.opid = UnusualRegister;
                            //Content argc;
                            std::string sregid = togen[i][_each_command].args[_each_arg].substr(3);
                            if(sregid == "fp") arg.c.intc = 0;
                            if(sregid == "sp") arg.c.intc = 1;
                            if(sregid == "pc") arg.c.intc = 2;
                            if(sregid == "sb") arg.c.intc = 3;
                            bytecode.push_back(arg);
                        }else{
                            arg.opid = NormalRegister;
                            //Content argc;
                            arg.c.intc = atol(togen[i][_each_command].args[_each_arg].substr(3).c_str());
                            bytecode.push_back(arg);
                        }
                    }else if(togen[i][_each_command].args[_each_arg][0] == '['){
                        //不会有傻逼拿特殊寄存器来放地址吧？不会吧不会吧？
                        std::string nstr = togen[i][_each_command].args[_each_arg].substr(1,togen[i][_each_command].args[_each_arg].length() - 1);
                        arg.opid = (nstr.substr(0,3) == "reg") ? Address_Register : Address;
                        arg.c.intc = (nstr.substr(0,3) == "reg") ? atol(nstr.substr(3).c_str()) : atol(nstr.c_str());
                        bytecode.push_back(arg);
                    }else if(function_table.count(togen[i][_each_command].args[_each_arg])){
                        long s = 0;
                        while(togen[s++].name != togen[i][_each_command].args[_each_arg]);
                        arg.opid = Number; //函数表编号，Number
                        arg.c.intc = --s;
                        bytecode.push_back(arg);
                    }else{
                        arg.opid = Number;
                        arg.c.intc = atol(togen[i][_each_command].args[_each_arg].c_str());
                        bytecode.push_back(arg);
                    }
                    bytecode_top ++;
                }
            }
        }
        ByteCode a;
        a.c.intc = INT_MAX;
        a.opid = Command;
        bytecode.push_back(a);
        bytecode_top++;
    }
    VMExec packVMExec(){
        VMExec ret;
        ret.cpool = cp;
        ret.label_array = codelbls;
        ret.code_array = (ByteCode*) bytecode.data();
        ret.head.support_vm_build = 0001;
        ret.head.hash = 0x114514ff;
        ret.head.code_label_count = togen.size();
        ret.head.code_length = bytecode.size();
        return ret;
    }
};