#include "core.cpp"
#include "../compiler/core.cpp"

int main(int argc,char** argv){
    std::string path = "test.xvm";
    //getline( std::cin,path);
    VMExec vme;
    VMRuntime vmr(vme,LoadVMExec((char*)path.c_str(),vme));
    vmr.vm_rules["verbose"] = true;
    for(int i = 1;i < argc;i++){
        if(vmr.vm_rules["verbose"] == true) std::cout << "Loading Device From:" << argv[i] << std::endl;
        devhost.LoadVMDevice(&vmr,argv[i]);
    }
    //try{
    //disasm(vme.code_array,vme.head.code_length);
    vmr.Run();
    // /DebugOutput(vmr);
    while(!std::cin.eof()){
        std::cout << "(debugger) > ";
        std::string s;
        std::getline(std::cin,s);
        Lexer lex(s);
        std::string main = lex.getNextToken().str;
        if(main == "debug_output") DebugOutput(vmr,std::cout);
        else if(main == "mem") Memory_Watcher(vmr,atol(lex.getNextToken().str.data()),std::cout);
        else if(main == "exit") break;
    }
    /*}catch(VMError e){
        e.what();
        std::cout << "ERRCOMMAND: ";
        vmr.disasm();
    }*/
    return 0;
}