#include "compiler/compiler.cpp"
using namespace std;

int main(){
    string code;
    ofstream output("compile_log.log");
    string filename;
    getline(cin,filename);
    if(filename == "") return 1;
    ifstream infile; 
    infile.open(filename);
    output << "Xlang Compiler Alpha 0.1\n";
    output << "Powered by xiaokang00010\n";
    output << "Filename -> " << filename << endl;
    output << "Reading Program to buffer...";
    while(!infile.eof()){
        string tmpline;
        getline(infile,tmpline);
        code += tmpline + "\n";
    }
    output << code << endl;
    InitCompiler();
    vector<ASMBlock> asms;
    try{
        asms = CompileProcess(code);
    }
    catch (CompileError e){
        e.what();
    }catch(ParserError e){
        e.what();
    }
    for(int i = 0;i < asms.size();i=i+1){
        cout << asms[i].toString();
    }
    cout << endl;
}