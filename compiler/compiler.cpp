#include "core.cpp"
#include "../vm/core.cpp"
using namespace std;

class CompileError{
    string msg;
    public:
    CompileError(string msg){this->msg = msg;}
    void what(){
        cerr << "\033[31m[CompileError] There was an error has been throwed in compile process:\n" << msg << "\033[0m\n";
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
    TypeName(){}
    TypeName(string name,VarType type){
        this->name = name;
        this->type = type;
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

addr_t sp;

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
        return "_" + type.name + funcname;
    }
} function_definition;

map<string,Symbol> symbol_table;
map<string,ASTree> function_table;

string getFunctionRealName(ASTree a,TypeName& this_scope){
    if(!ASTree_APIs::MemberExpression::hasFunctionCallStatement(a))  return ""; // It's a stupid fix.But,It's work now.
    else{
        if(a.node[1].nodeT == FunctionCallStatement) return "_@" + this_scope.name  + "_" + a.node[1].this_node.str;
        else{
            return getFunctionRealName(a.node[1],this_scope[a.node[1].node[0].this_node.str]);
        }
    }
}

string getFunctionRealName(ASTree a){
    if(a.nodeT != ExpressionStatement) return "";
    if(type_pool.find(a.node[0].this_node.str) == type_pool.end()) return "";
    return getFunctionRealName(a,type_pool[a.node[0].this_node.str]);
}

ASTree getFunctionCallArgs(ASTree ast){
    if(ast.nodeT == FunctionCallStatement) return ast.node[0];
    else if(ast.nodeT == ExpressionStatement && ast.this_node.type == TOK_DOT) return getFunctionCallArgs(ast.node[1]);
}

ConstantPool cp;

namespace ConstPool_Apis
{
    void Init(ConstantPool cpool,size_t size = 1024){
        cpool.pool = (char*)realloc(cpool.pool,size);
        cpool.items = (size_t*)malloc(1024);
        memset(cpool.items,0,1024);
    }
    addr_t Insert(ConstantPool cpool,char* _Src,addr_t size){
        memcpy(cpool.pool + cpool.items[cpool.count],_Src,size);
        if(cpool.count == 0){
            cpool.items[cpool.count] = size;
            return cpool.items[cpool.count];
        }
        cpool.count++;
        cpool.items[cpool.count] = cpool.items[cpool.count-1] + size;
        return cpool.items[cpool.count-1];
    }
} // namespace ConstPool_Apis

string guessType(ASTree ast){
    if(ast.nodeT == Id){
        if(ast.this_node.type == TOK_ID && symbol_table.find(ast.this_node.str) != symbol_table.end())  return symbol_table[ast.this_node.str]._Typename;
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
    }else if(ast.nodeT == ExpressionStatement){
        if(ast.nodeT == TOK_DOT){
            return type_pool[symbol_table[ast.node[0].this_node.str]._Typename].findObject(ast.node[1]).name;
        }else if(ast.nodeT == TOK_COLON){
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
    return getMemberType(ast.node[1],scope[ast.node[1].node[0].this_node.str]);
}
VarType getMemberType(ASTree ast){
    return getMemberType(ast,type_pool[ast.node[0].this_node.str]);
}

ASMBlock dumpToAsm(ASTree ast){
    if(ast.nodeT == Id){
        if(ast.this_node.type == TOK_INTEGER || ast.this_node.type == TOK_DOUBLE){
            return ASMBlock().genCommand("mov").genArg(ast.this_node.str).genArg("reg" + to_string(getLastUsingRegId())).push();
        }
        if(ast.this_node.type == TOK_STRING){
            return ASMBlock().genCommand("@constant_pool").genArg(ast.this_node.str)\
            .genCommand("mov").genArg(to_string(cp.items[cp.count])).genArg("reg" + to_string(getLastUsingRegId())).push();
        }
        if(symbol_table.find(ast.this_node.str) != symbol_table.end()){
            return ASMBlock().genCommand("mov").genArg(to_string(symbol_table[ast.this_node.str].frame_position)).genArg("reg" + to_string(getLastUsingRegId()));
        }
        if(ast.this_node.str == "true")  return ASMBlock().genCommand("mov").genArg("1").genArg("reg" + to_string(getLastUsingRegId()));
        if(ast.this_node.str == "false")  return ASMBlock().genCommand("mov").genArg("0").genArg("reg" + to_string(getLastUsingRegId()));
    }
    if(ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast)){
        string func_name = getFunctionRealName(ast);
        ASTree args = getFunctionCallArgs(ast);
        ASMBlock asb;
        asb.genCommand("save").push();
        if(args.nodeT == NormalStatement && args.this_node.type == TOK_ARGSTATEMENT){
            for(int i = 0;i < args.node.size();i++){
                asb += dumpToAsm(args.node[i]);
                asb.genCommand("push").genArg("[" + to_string(getLastUsingRegId()) + "]").genArg(to_string(getMemberSize(args.node[i]))).push();
            }
        }
        return asb.genCommand("call").genArg(func_name);
    }
    if(ast.nodeT == ExpressionStatement){
        if(ast.this_node.type == TOK_PLUS || ast.this_node.type == TOK_MINUS || ast.this_node.type == TOK_MULT || ast.this_node.type == TOK_DIV){
            ASMBlock ab;
            ab += dumpToAsm(ast.node[0]);
            if(ast.node[0].nodeT == FunctionCallStatement || ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[0])) ab.genCommand("pop").genArg("reg" + to_string(getLastUsingRegId()));
            RegState[getLastUsingRegId()] = true;
            ab += dumpToAsm(ast.node[1]);
            if(ast.node[1].nodeT == FunctionCallStatement || ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[1])) ab.genCommand("pop").genArg("reg" + to_string(getLastUsingRegId()));
            RegState[getLastUsingRegId()] = true;
            if(ast.this_node.type == TOK_PLUS) ab.genCommand("add");
            else if(ast.this_node.type == TOK_MINUS) ab.genCommand("sub");
            else if(ast.this_node.type == TOK_MULT) ab.genCommand("mul");
            else if(ast.this_node.type == TOK_DIV) ab.genCommand("div");
            // Xlang变量的dumpASM会传地址
            // 加减乘除默认8byte运算
            if(ast.node[0].nodeT == TOK_INTEGER || ast.node[0].nodeT == TOK_DOUBLE || ast.node[0].nodeT == TOK_CHARTER ) ab.genArg("reg" + to_string(getLastUsingRegId() - 2));
            else if(ast.node[0].nodeT == FunctionCallStatement || ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[0])) ab.genArg("reg" + to_string(getLastUsingRegId() - 2));
            else if(getMemberType(ast.node[0]) == __BASIC_8BYTE || getMemberType(ast.node[0]) == __BASIC_1BYTE) ab.genArg("[reg" + to_string(getLastUsingRegId() - 2) + "]");
            else throw CompileError("TypeError: 加减乘除仅限于基础类型,除非你想让虚拟机崩掉.\nBasic operator only support basic types,if you want to let the vm crash then don't do it.");
            if(ast.node[1].nodeT == TOK_INTEGER || ast.node[1].nodeT == TOK_DOUBLE || ast.node[1].nodeT == TOK_CHARTER) ab.genArg("reg" + to_string(getLastUsingRegId() - 1));
            else if(ast.node[1].nodeT == FunctionCallStatement || ASTree_APIs::MemberExpression::hasFunctionCallStatement(ast.node[1])) ab.genArg("reg" + to_string(getLastUsingRegId() - 1));
            else if(getMemberType(ast.node[1]) == __BASIC_8BYTE || getMemberType(ast.node[1]) == __BASIC_1BYTE) ab.genArg("[reg" + to_string(getLastUsingRegId() - 1) + "]");
            else throw CompileError("TypeError: 加减乘除仅限于基础类型,除非你想让虚拟机崩掉.\nBasic operator only support basic types,if you want to let the vm crash then don't do it.");
            RegState[getLastUsingRegId() - 1] = false;
            RegState[getLastUsingRegId() - 1] = false;
            return ab;
        }
        if(ast.this_node.type == TOK_DOT){
            // address only
            int fp_offset = type_pool[symbol_table[ast.node[0].this_node.str]._Typename].getOffset(ast.node[1],symbol_table[ast.node[0].this_node.str].frame_position);
            return ASMBlock().genCommand("mov").genArg("reg" + getLastUsingRegId()).genArg(to_string(fp_offset)).push();
        }
    }
    if(ast.nodeT == NormalStatement){
        if(ast.this_node.str == "struct"){
            string struct_name = ast.node[0].this_node.str;
            TypeName t(struct_name,__OBJECT);
            ASTree& contents = ast.node[1];
            for(int i = 0;i < contents.node.size();i++){
                if(contents.node[i].this_node.str == "func"){
                    // TODO: add function definition processing core
                    string real_funcname = "_@" + struct_name + "_" + contents.node[i].node[0].node[0].this_node.str;
                    if(contents.node[i].node[1].nodeT != BlockStatement) throw CompileError("func definition statement must have a codeblock!");
                    function_table[function_definition(real_funcname,type_pool[contents.node[i].node[0].this_node.str]).getRealname()] = contents.node[i].node[1];
                    continue;
                }
                if(type_pool.find(contents.node[i].this_node.str) == type_pool.end()) throw CompileError(contents.node[i].this_node.str + " doesn't an exist typename.");
                TypeName this_type = type_pool[contents.node[i].this_node.str];
                ASTree& temp_ast = contents.node[i];
                for(int j = 0;j < temp_ast.node.size();j++){
                    if(temp_ast.node[j].nodeT == Id) t.InsertToObject(temp_ast.node[j].this_node.str,this_type);
                    else throw CompileError("Compiler doesn't support init value now"); // TODO: Add init value support
                }
            }
            type_pool["ptr_" + struct_name] = TypeName("ptr_"+struct_name,__BASIC_8BYTE);
            return ASMBlock();
        }
        if(ast.this_node.str == "func"){
            string real_funcname = ast.node[0].node[0].this_node.str;
            function_definition fdef(real_funcname,type_pool[ast.node[0].this_node.str]);
            if(ast.node[1].nodeT != BlockStatement) throw CompileError("Function Definition Statemet must have an block statement");
            function_table[fdef.getRealname()] = ast.node[1];
            return ASMBlock();
        }
        if(type_pool.find(ast.this_node.str) != type_pool.end()){
            TypeName& typen = type_pool[ast.this_node.str];
            ASMBlock asb;
            for(int i = 0;i < ast.node.size();i++){
                if(ast.node[i].nodeT == Id){
                    symbol_table[ast.node[i].this_node.str] = Symbol(typen.name);
                }
                if(ast.node[i].nodeT == ExpressionStatement && ast.node[i].this_node.type == TOK_EQUAL){
                    // has init value
                    asb += dumpToAsm(ast.node[i].node[1]);
                    symbol_table[ast.node[i].node[0].this_node.str] = Symbol(typen.name);
                    asb.genCommand("mov_m").genArg(to_string(symbol_table[ast.node[i].node[0].this_node.str].frame_position)).genArg("[reg" + to_string(getLastUsingRegId()) + "]").genArg(to_string(typen.size));
                }
            }
        }
        throw CompileError("Unknown Command: " + ast.this_node.str);
    }
    if(ast.nodeT == BlockStatement){
        ASMBlock ret;
        for(int i = 0;i < ast.node.size();i++) ret += dumpToAsm(ast.node[i]);
        return ret;
    }
}