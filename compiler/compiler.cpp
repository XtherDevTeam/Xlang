#include "core.cpp"
#include "../vm/core.cpp"
using namespace std;

struct ASM_Command{
    string Main;
    vector<string> args;
    string toString(){
        stringstream ss;
        ss << Main << " ";
        for(int i = 0;i < args.size();i++){
            ss << args[i] << ",";
        }
        string ret = ss.str();
        ret.erase(ret.length() - 1);
        ret += ";\n";
        return ret;
    }
};

class ASMBlock{
    ASM_Command temp;
    public:
    string name;
    vector<ASM_Command> lists;
    ASMBlock(){}
    void operator+=(ASMBlock o){
        if(o.lists.empty()) return; 
        for(int i = 0;i < o.lists.size();i++){
            lists.push_back(o.lists[i]);
        }
    }
    ASM_Command& operator[](int index){return lists[index];}
    ASMBlock& genCommand(string s){
        if(temp.Main != ""){
            lists.push_back(temp);
            temp.Main = "";
            temp.args.clear();
        }
        temp.Main = s;
        return *this;
    }
    ASMBlock& genArg(string s){
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
    string toString(){
        stringstream ss;
        ss << name << ":" << endl;
        for(int i = 0;i < lists.size();i++){
            ss << lists[i].toString();
        }
        return ss.str();
    }
};

string guessType(ASTree ast);

enum dumpToAsmState{
    CaseMode,
    GlobalMode,
    LoopMode,
};

class CompileError{
    string msg;
    public:
    CompileError(string msg){this->msg = msg;}
    void what(){
        cerr << "\033[31m[CompileError]\033[0m There was an error has been throwed in compile process:\n" << msg << "\033[0m\n";
    }
};

enum VarType{
    __BASIC_1BYTE,
    __BASIC_8BYTE,
    __OBJECT
};

class TypeName{
    public:
    string  name;
    VarType type;
    int size;
    map<string,TypeName> objects;

    TypeName& operator[](string s){
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
    TypeName(string name,VarType type){
        this->name = name;
        this->type = type;
        if(type == __BASIC_8BYTE) this->size = 8;
        else if(type == __BASIC_1BYTE) this->size = 1;
        else this->size = 0;
    }
    void InsertToObject(string s,TypeName t){
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

map<string,TypeName> type_pool;

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
    SymbolType type;
    string _Typename;
    size_t frame_position;
    Symbol(string typename_){
        frame_position = sp;
        _Typename = typename_;
        if(type_pool.find(_Typename) == type_pool.end()) throw CompileError("Undefined Typename as " + _Typename);
        sp += type_pool[_Typename].size;
    }
    Symbol(){}
};

typedef class Function_{
    public:
    string funcname;
    TypeName type;
    Function_(){}
    Function_(string s,TypeName t){
        funcname = s;
        type = t;
    }
    string getRealname(){
        return (funcname[0] == '_' ? funcname : "_") + funcname + "_" + type.name;
    }
} function_definition;

class function_block{
    public:
    addr_t restore_symboltop;
    ASTree type_and_args;
    ASTree block_statement;
    map<string,Symbol> syminfunc;
    function_block(ASTree args,ASTree bls){
        restore_symboltop = 0;
        addr_t tsp = sp;sp = 0;
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

map<string,Symbol> global_symbol_table; // 存放在常量池里的全局符号表
map<string,Symbol> symbol_table;
map<string,function_block> function_table;

string getFunctionRealName(ASTree a,TypeName& this_scope){
    if(!ASTree_APIs::MemberExpression::hasFunctionCallStatement(a))  return ""; // It's a stupid fix.But,It's work now.
    else{
        if(a.node[1].nodeT == FunctionCallStatement) return "_@" + this_scope.findObject(a.node[0]).name  + "_" + a.node[1].this_node.str;
        else{
            return getFunctionRealName(a.node[1],this_scope[a.node[0].this_node.str]);
        }
    }
}


string getFunctionRealName(ASTree a){
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

string funcnameInTab(string realname){
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
            return cpool.count++;
        }
        cpool.count++;
        cpool.items[cpool.count] = cpool.items[cpool.count-1] + size;
        return cpool.count-1;
    }
} // namespace ConstPool_Apis

bool isNormalExpression(ASTree ast){
    if(ast.this_node.type != TOK_DOT && ast.this_node.type != Id && ast.this_node.type != TOK_ID && ast.this_node.type != TOK_STRING) return true;
    return false;
}

string guessType(ASTree ast){
    if(ast.nodeT == Id){
        if(ast.this_node.type == TOK_ID && symbol_table.count(ast.this_node.str))  return symbol_table[ast.this_node.str]._Typename;
        else if(global_symbol_table.count(ast.this_node.str)) return global_symbol_table[ast.this_node.str]._Typename;
        switch (ast.this_node.type)
        {
        case TOK_STRING:
            return "string";
        
        case TOK_INTEGER:
            return "int";
        
        case TOK_DOUBLE:
            return "double";
        
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
            if(symbol_table.count(ast.node[0].this_node.str)) return type_pool[symbol_table[ast.node[0].this_node.str]._Typename].findObject(ast.node[1]).name;
            else if(global_symbol_table.count(ast.node[0].this_node.str)) return type_pool[global_symbol_table[ast.node[0].this_node.str]._Typename].findObject(ast.node[1]).name;
            else throw CompileError(ast.node[0].this_node.str + " doesn't exist");
        }else if(ast.this_node.type == TOK_COLON){
            return ast.node[0].this_node.str; // 送 业 绩
        }else{
            if(guessType(ast.node[0]) == "string" || guessType(ast.node[1]) == "string") return "string";
            return guessType(ast.node[0]);
        }
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
            return ASMBlock().genCommand("mov").genArg("reg" + to_string(getLastUsingRegId())).genArg(ast.this_node.str).push();
        }
        if(ast.this_node.type == TOK_STRING){
            return ASMBlock().genCommand("mov").genArg("reg" + to_string(getLastUsingRegId())).genArg(to_string(ConstPool_Apis::Insert(cp,(char*)ast.this_node.str.c_str(),ast.this_node.str.size()))).push();
        }
        if(symbol_table.find(ast.this_node.str) != symbol_table.end()){
            return ASMBlock().genCommand("mov").genArg("reg" + to_string(getLastUsingRegId())).genArg(to_string(symbol_table[ast.this_node.str].frame_position)).genCommand("mov").genArg("reg"+to_string(getLastUsingRegId())).genArg("regsb").genCommand("sub").genArg("reg"+to_string(getLastUsingRegId())).genArg("regfp").push();
        }
        if(global_symbol_table.find(ast.this_node.str) != global_symbol_table.end()){
            return ASMBlock().genCommand("mov").genArg("reg" + to_string(getLastUsingRegId())).genArg(to_string(global_symbol_table[ast.this_node.str].frame_position)).push();
        }
        if(ast.this_node.str == "true")  return ASMBlock().genCommand("mov").genArg("reg" + to_string(getLastUsingRegId())).genArg("1").push();
        if(ast.this_node.str == "false")  return ASMBlock().genCommand("mov").genArg("reg" + to_string(getLastUsingRegId())).genArg("0").push();
    }
    if(ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast)){
        string func_name = getFunctionRealName(ast);
        ASTree args = getFunctionCallArgs(ast);
        ASMBlock asb;
        asb.genCommand("save").push();
        if(args.nodeT == Args){
            ASTree type_a_names = function_table[funcnameInTab(func_name)].type_and_args;
            if(args.node.size() != type_a_names.node.size()) throw CompileError("Too few/much args have been gave.");
            for(int i = 0;i < args.node.size();i++){
                asb += dumpToAsm(args.node[i]);
                string realarg0 = "reg" + to_string(getLastUsingRegId());
                //cout << "BUGHERE:" << AST_nodeType[ast.node[i].node[0].nodeT] << endl;
                if(ast.node[i].this_node.type == TOK_INTEGER || ast.node[i].this_node.type == TOK_DOUBLE || ast.node[i].this_node.type == TOK_CHARTER ) /*do nothing*/;
                else realarg0 = "[" + realarg0 + "]";
                asb.genCommand("push").genArg(realarg0).genArg(to_string(getMemberSize(args.node[i]))).push();
            }
        }
        //cout << "\033[32mGuess Result:\033[0m" << guessType(ast) << " " << getFunctionRealName(ast) << endl;
        asb.genCommand("call").genArg(funcnameInTab(func_name)).\
        genCommand("mov").genArg("reg" + to_string(getLastUsingRegId())).genArg("stackbase").\
        genCommand("sub").genArg("reg" + to_string(getLastUsingRegId())).genArg("regfp").\
        genCommand("sub").genArg("reg" + to_string(getLastUsingRegId())).genArg("regsp").\
        // genCommand("add").genArg("reg" + to_string(getLastUsingRegId())).genArg(to_string(getMemberSize(ast))).\  低端序，直接从下面读到上面
        genCommand("add").genArg("regsp").genArg(to_string(getMemberSize(ast))).push();
        sp += getMemberSize(ast);
        return asb;
    }
    if(ast.nodeT == ExpressionStatement){
        if(ast.this_node.type == TOK_PLUS || ast.this_node.type == TOK_MINUS || ast.this_node.type == TOK_MULT || ast.this_node.type == TOK_DIV || ast.this_node.type == TOK_2EQUAL || ast.this_node.type == TOK_MAXEQUAL || ast.this_node.type == TOK_MINEQUAL || ast.this_node.type == TOK_MAX || ast.this_node.type == TOK_MIN){
            ASMBlock ab;
            ab += dumpToAsm(ast.node[0]);
            //if(ast.node[0].nodeT == FunctionCallStatement || ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[0])) ab.genCommand("").genArg("reg" + to_string(getLastUsingRegId()));
            RegState[getLastUsingRegId()] = true;
            ab += dumpToAsm(ast.node[1]);
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
            // Xlang变量的dumpASM会传地址
            // 加减乘除默认8byte运算
            if(isNormalExpression(ast.node[0])) ab.genArg("reg" + to_string(getLastUsingRegId() - 2));
            else if(ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[0])){
                ab.genArg("[reg" + to_string(getLastUsingRegId() - 2) + "]");
                sp -= getMemberSize(ast.node[0]);
            }
            else if(getMemberType(ast.node[0]) == __BASIC_8BYTE || getMemberType(ast.node[0]) == __BASIC_1BYTE) ab.genArg("[reg" + to_string(getLastUsingRegId() - 2) + "]");
            else throw CompileError("TypeError: 加减乘除以及逻辑运算仅限于基础类型,除非你想让虚拟机崩掉.\nBasic operator and boolean expression only support basic types,if you want to let the vm crash then don't do it.");
            if(isNormalExpression(ast.node[1])) ab.genArg("reg" + to_string(getLastUsingRegId() - 1));
            else if(ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[1])) ab.genArg("[reg" + to_string(getLastUsingRegId() - 1) + "]");
            else if(getMemberType(ast.node[1]) == __BASIC_8BYTE || getMemberType(ast.node[1]) == __BASIC_1BYTE) ab.genArg("[reg" + to_string(getLastUsingRegId() - 1) + "]");
            else throw CompileError("TypeError: 加减乘除以及逻辑运算仅限于基础类型,除非你想让虚拟机崩掉.\nBasic operator and boolean expression only support basic types,if you want to let the vm crash then don't do it.");
            RegState[getLastUsingRegId() - 1] = false;
            RegState[getLastUsingRegId() - 1] = false;
            return ab.push();
        }
        if(ast.this_node.type == TOK_DOT){
            // address only
            int fp_offset = type_pool[symbol_table[ast.node[0].this_node.str]._Typename].getOffset(ast.node[1],symbol_table[ast.node[0].this_node.str].frame_position);
            return ASMBlock().genCommand("mov").genArg("reg" + getLastUsingRegId()).genArg(to_string(fp_offset)).genCommand("mov").genArg("reg"+to_string(getLastUsingRegId())).genArg("regsb").genCommand("sub").genArg("reg" + to_string(getLastUsingRegId())).genArg("regfp").push();
        }
        if(ast.this_node.type == TOK_EQUAL){
            ASMBlock asb;
            asb += dumpToAsm(ast.node[0]);
            RegState[getLastUsingRegId()] = true;
            asb += dumpToAsm(ast.node[1]);
            RegState[getLastUsingRegId() - 1] = false;
            asb.genCommand("mov_m").genArg("[reg" + to_string(getLastUsingRegId()) + "]").genArg("[reg" + to_string(getLastUsingRegId()+1) + "]").genArg(to_string(getMemberSize(ast.node[1]))).push();
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
            string struct_name = ast.node[0].this_node.str;
            TypeName t(struct_name,__OBJECT);
            ASTree& contents = ast.node[1];
            type_pool["ptr_" + struct_name] = TypeName("ptr_"+struct_name,__BASIC_8BYTE);
            type_pool["ptr_" + struct_name].size = 8;
            for(int i = 0;i < contents.node.size();i++){
                if(contents.node[i].this_node.str == "func"){
                    // TODO: add function definition processing core
                    string real_funcname = "_@" + struct_name + "_" + contents.node[i].node[0].node[0].this_node.str;
                    //if(contents.node[i].node[2].nodeT != BlockStatement) throw CompileError("func definition statement must have a codeblock!");
                    //function_table[function_definition(real_funcname,type_pool[contents.node[i].node[0].this_node.str]).getRealname()] = function_block(contents.node[i].node[1],contents.node[i].node[2]);
                    function_definition fdef(real_funcname,type_pool[contents.node[i].node[0].this_node.str]);
                    if(contents.node[i].node[2].nodeT != BlockStatement) throw CompileError("Function Definition Statemet must have an block statement");
                    function_table[fdef.getRealname()] = function_block(contents.node[i].node[1],contents.node[i].node[2]);
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
            string real_funcname = ast.node[0].node[0].this_node.str;
            function_definition fdef(real_funcname,type_pool[ast.node[0].this_node.str]);
            if(ast.node[2].nodeT != BlockStatement) throw CompileError("Function Definition Statemet must have an block statement");
            function_table[fdef.getRealname()] = function_block(ast.node[1],ast.node[2]);
            return ASMBlock();
        }
        if(type_pool.count(ast.this_node.str)){
            TypeName& typen = type_pool[ast.this_node.str];
            ASMBlock asb;
            for(int i = 0;i < ast.node.size();i++){
                if(ast.node[i].nodeT == Id){
                    if(mode){
                        int cp_adr = ConstPool_Apis::Insert(cp,(char*)malloc(typen.size),typen.size);
                        global_symbol_table[ast.node[i].this_node.str].frame_position = cp.items[cp_adr]; // WARN: 挖坑
                        global_symbol_table[ast.node[i].this_node.str]._Typename = typen.name;
                        asb.genCommand("mov_m").genArg("[" + to_string(cp.items[cp_adr]) + "]").genArg(to_string(0)).genArg(to_string(typen.size));
                        continue;
                    }
                    symbol_table[ast.node[i].this_node.str] = Symbol(typen.name);
                    asb.genCommand("sub").genArg("regsp").genArg(to_string(typen.size)); // stack由上往下
                }
                if(ast.node[i].nodeT == ExpressionStatement && ast.node[i].this_node.type == TOK_EQUAL){
                    // has init value
                    asb += dumpToAsm(ast.node[i].node[1]);
                    if(mode){
                        string realarg0 = "reg" + to_string(getLastUsingRegId());
                        //cout << "BUGHERE:" << AST_nodeType[ast.node[i].node[0].nodeT] << endl;
                        if(ast.node[i].node[1].this_node.type == TOK_INTEGER || ast.node[i].node[1].this_node.type == TOK_DOUBLE || ast.node[i].node[1].this_node.type == TOK_CHARTER ) /*do nothing*/;
                        else realarg0 = "[" + realarg0 + "]";
                        global_symbol_table[ast.node[i].node[0].this_node.str].frame_position = cp.items[cp.count];
                        global_symbol_table[ast.node[i].node[0].this_node.str]._Typename = typen.name;
                        //cout << "BUGHERE:" << typen.name << endl;
                        ConstPool_Apis::Insert(cp,(char*)malloc(typen.size),typen.size);
                        asb.genCommand("mov_m").genArg("[" + to_string(cp.items[cp.count - 1]) + "]");
                        asb.genArg(realarg0);
                        asb.genArg(to_string(typen.size));
                        continue;
                    }
                    string realarg0 = "reg" + to_string(getLastUsingRegId());
                    if(ast.node[i].node[1].this_node.type  == TOK_INTEGER || ast.node[i].node[1].this_node.type  == TOK_DOUBLE || ast.node[i].node[1].this_node.type  == TOK_CHARTER ) /* do nothing*/;
                    else realarg0 = "[" + realarg0 + "]";
                    symbol_table[ast.node[i].node[0].this_node.str] = Symbol(typen.name);
                    asb.genCommand("push").genArg(realarg0).genArg(to_string(typen.size));
                    sp += typen.size;
                }
            }
            return asb.push();
        }
        if(ast.this_node.str == "if"){
            // regflag
            ASMBlock asb;
            if(ast.node[0].nodeT != Args || ast.node[1].nodeT != BlockStatement)  throw ParserError("SyntaxError: Bad If Statement");
            ASMBlock tmpblock = dumpToAsm(ast.node[1]);
            tmpblock.genCommand("_$fakecommand_goto_statement_end").push(); // end_pos - now_pos = goto command
            asb += dumpToAsm(ast.node[0].node[0]);
            asb.genCommand("gt").genArg("2").genCommand("gf").genArg(to_string(tmpblock.lists.size()+1)).push();
            asb += tmpblock;
            if(ast.node.size() <= 2) return asb;
            for(int i = 2;i < ast.node.size();i+=3){
                if(ast.node[i].this_node.str == "elif"){
                    ASMBlock tmpblock = dumpToAsm(ast.node[i+2]);
                    tmpblock.genCommand("_$fakecommand_goto_statement_end").push(); // end_pos - now_pos = goto command
                    asb += dumpToAsm(ast.node[i+1].node[0]);
                    asb.genCommand("gt").genArg("2").genCommand("gf").genArg(to_string(tmpblock.lists.size()+1)).push();
                    asb += tmpblock;
                }else if(ast.node[i].this_node.str == "else"){
                    asb += dumpToAsm(ast.node[i+1]);
                }else{
                    throw CompileError("Unknown Command:" + ast.node[i].this_node.str);
                }
            }
            for(int i = 0;i < asb.lists.size();i++){
                if(asb.lists[i].Main == "_$fakecommand_goto_statement_end"){
                    asb.lists[i].Main = "goto";
                    asb.lists[i].args.push_back(to_string( asb.lists.size() - 1 - i + 1 ));
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
            gt 2;
            gf 6; // 4 = blocksize(2) + endofblocksize(3) + 1

            add [8],1;
            goto 1; // blocksize - itself, 2 - 1 = 1

            add reg0,1;
            add reg1,1;
            goto -7; // i - 1 - inital_val(2)
            */
            ASMBlock asb,inital_val,boolean_expression,for_blockstmt,endofblock;
            if(ast.node[0].nodeT != Args || ast.node[1].nodeT != BlockStatement)  throw ParserError("SyntaxError: Bad For Statement");

            inital_val = dumpToAsm(ast.node[0].node[0]);

            boolean_expression = dumpToAsm(ast.node[0].node[1]);

            for_blockstmt = dumpToAsm(ast.node[1]);

            endofblock = dumpToAsm(ast.node[0].node[2]);
            endofblock.genCommand("_$fakecommand_goto_loop_start").push();

            boolean_expression.genCommand("gt").genArg("2").genCommand("gf").genArg(to_string(for_blockstmt.lists.size() + endofblock.lists.size())).push();

            asb += inital_val;
            asb += boolean_expression;
            asb += for_blockstmt;
            asb += endofblock;

            for(int i = 0;i < asb.lists.size();i++){
                if(asb.lists[i].Main == "_$fakecommand_goto_loop_start"){
                    asb.lists[i].Main = "goto";
                    asb.lists[i].args.push_back("-" + to_string( (int)(i - 1 - inital_val.lists.size()) ));
                }
                if(asb.lists[i].Main == "_$fakecommand_loop_continue"){
                    asb.lists[i].Main = "goto";
                    asb.lists[i].args.push_back(to_string( asb.lists.size() - endofblock.lists.size() - i + 1));
                }
                if(asb.lists[i].Main == "_$fakecommand_goto_for_end"){
                    asb.lists[i].Main = "goto";
                    //cout << "\033[31mwtf>>" << asb.lists.size() - i << endl;
                    asb.lists[i].args.push_back(to_string( asb.lists.size() - i ));
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
            string realarg0 = "reg" + to_string(getLastUsingRegId());
            if(ast.node[0].this_node.type == TOK_INTEGER || ast.node[0].this_node.type == TOK_DOUBLE || ast.node[0].this_node.type == TOK_CHARTER ) /*do nothing*/;
            else realarg0 = "[" + realarg0 + "]";
            return asb.genCommand("push").genArg(realarg0).genArg(to_string(getMemberSize(ast.node[0]))).push();
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

vector<ASMBlock> CompileProcess(string code){
    try{
    ASMBlock asb;
    int intext=0,block=0,brack1=0,brack2=0,brack3=0;
    string tmp;
    for(int i = 0;i < code.length();i++){
        if(code[i] == '\\') {
            if(code[i+1] == '\n') i+=2;
            else if(code[i+1] == '\r') i+=3;
            else if(code[i+1] == 'n'){tmp+='\n';i+=2;}
            else continue;
        }
        else if(code[i] == '\n' || code[i] == '\r') continue;
        else if(code[i] == '\"') intext = !intext;
        else if(code[i] == '(') brack1++; else if(code[i] == ')') brack1--;
        else if(code[i] == '[') brack2++; else if(code[i] == ']') brack2--;
        else if(code[i] == '{') brack3++; else if(code[i] == '}') brack3--;
        if(code[i] == ';' && brack1 == 0 && brack2 == 0 && brack3 == 0 && !intext) {
            //cout << "HERE:" << tmp;
            Lexer lex(tmp);
            asb += dumpToAsm(ASTree(lex),true);
            tmp = "";
            continue;
        }
        tmp += code[i];
    }
    vector<ASMBlock> asblst;
    // 默认起始点为main函数
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
    }
    catch(CompileError e){
        e.what();
    }
    catch(ParserError e){
        e.what();
    }
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
    string COMMAND_MAP[] = {
        "mov","mov_m","push","pop","save","pop_frame",
        "add","sub","mul","div",
        "equ","maxeq","mineq","max","min",
        "goto","gt","gf"
    };
    int getCommandId(string command){
        for(int i = 0;i < 18;i=i+1){
            if(command == COMMAND_MAP[i]) return i;
        }
        return INT_MAX;
    }
    vector<ASMBlock> togen;
    CodeLabel *codelbls;
    int bytecode_top = 0;
    string bytecode;
    void Init(vector<ASMBlock> &blocks){
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
            strcpy(codelbls[i].label_n,togen[i].name.c_str());
            codelbls[i].start = bytecode_top;
            for(int _each_command = 0;_each_command < togen[i].lists.size();_each_command++){
                bytecode += Command;
                Content c;
                c.intc = getCommandId(togen[i][_each_command].Main);
                bytecode += string(c.chc,8);
                bytecode_top += 9;
                for(int _each_arg = 0;_each_arg < togen[i][_each_command].args.size();_each_arg++){
                    if(togen[i][_each_command].args[_each_arg].substr(0,3) == "reg"){
                        if(togen[i][_each_command].args[_each_arg].substr(3) == "fp" || togen[i][_each_command].args[_each_arg].substr(3) == "sp" || togen[i][_each_command].args[_each_arg].substr(3) == "pc" || togen[i][_each_command].args[_each_arg].substr(3) == "sb"){
                            bytecode += UnusualRegister;
                            Content argc;
                            string sregid = togen[i][_each_command].args[_each_arg].substr(3);
                            if(sregid == "fp") argc.intc = 0;
                            if(sregid == "sp") argc.intc = 1;
                            if(sregid == "pc") argc.intc = 2;
                            if(sregid == "sb") argc.intc = 3;
                            bytecode += string(argc.chc,8);
                        }else{
                            bytecode += NormalRegister;
                            Content argc;
                            argc.intc = atoi(togen[i][_each_command].args[_each_arg].substr(3).c_str());
                            bytecode += string(argc.chc,8);
                        }
                    }else if(togen[i][_each_command].args[_each_arg][0] == '['){
                        //不会有傻逼拿特殊寄存器来放地址吧？不会吧不会吧？
                        string nstr = togen[i][_each_command].args[_each_arg].substr(1,togen[i][_each_command].args[_each_arg].length() - 1);
                        bytecode += (nstr.substr(0,3) == "reg") ? Address_Register : Address;
                        Content argc;
                        argc.intc = (nstr.substr(0,3) == "reg") ? atoi(nstr.substr(3).c_str()) : atoi(nstr.c_str());
                        bytecode += string(argc.chc,8);
                    }else{
                        bytecode += Number;
                        Content argc;
                        argc.intc = atoi(togen[i][_each_command].args[_each_arg].c_str());
                        bytecode += string(argc.chc,8);
                    }
                    bytecode_top += 9;
                }
            }
        }
    }
    VMExec packVMExec(){
        VMExec ret;
        ret.cpool = cp;
        ret.label_array = codelbls;
        ret.code_array = (ByteCode*) bytecode.c_str();
        ret.head.support_vm_build = 0001;
        ret.head.hash = 0x114514ff;
        ret.head.code_label_count = togen.size();
        ret.head.code_length = bytecode.size() / 9;
        return ret;
    }
};