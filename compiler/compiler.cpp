#include "core.cpp"
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
    size_t getOffset(ASTree ast,size_t originoffset){
        if(ast.nodeT != ExpressionStatement || ast.this_node.str != "."){
            if(ast.nodeT == Id){
                return originoffset;
            }
            throw ParserError("Not an member expression statement");
        }else{
            return operator[](ast.node[0].this_node.str).getOffset(ast.node[1],originoffset + operator[](ast.node[0].this_node.str).size);
        }
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
        _Typename = typename_;
        if(type_pool.find(_Typename) == type_pool.end()) throw CompileError("Undefined Typename as " + _Typename);
    }
};

map<string,Symbol> symbol_table;

string getFunctionRealName(ASTree a,TypeName& this_scope){
    if(!ASTree_APIs::MemberExpression::hasFunctionCallStatement(a))  return "";
    else{
        if(a.node[1].nodeT == FunctionCallStatement) return "_@" + this_scope.name  + "_" + a.node[1].this_node.str;
        else{
            return getFunctionRealName(a.node[1],this_scope[a.node[1].node[0].this_node.str]);
        }
    }
}