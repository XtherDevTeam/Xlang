#include "core.cpp"
 

int main(){
    std::string path = "test.xvm";
    //getline( std::cin,path);
    VMExec vme;
    LoadVMExec((char*)path.c_str(),&vme);
    VMRuntime vmr(vme);
    //try{
    vmr.vm_rules["verbose"] = true;
    //disasm(vme.code_array,vme.head.code_length);
    vmr.Run();
    // /DebugOutput(vmr);
    Memory_Watcher(vmr,0);
    /*}catch(VMError e){
        e.what();
        std::cout << "ERRCOMMAND: ";
        vmr.disasm();
    }*/
    return 0;
}