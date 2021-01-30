#include "compiler/compiler.cpp"
using namespace std;

int main(){
    InitCompiler();
    function_table[function_definition("_less",type_pool["int"]).getRealname()].block_statement = ASTree();
    cout << funcnameInTab("_less");
}
