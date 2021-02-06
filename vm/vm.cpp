#include "core.cpp"
 

int main(){
    std::string path = "test.xvm";
    //getline( std::cin,path);
    VMExec vme;
    LoadVMExec((char*)path.c_str(),&vme);
    VMRuntime vmr(vme);
    try{
    vmr.vm_rules["verbose"] = true;
    vmr.Run();
    DebugOutput(vmr);
    Memory_Watcher(vmr,0);
    }catch(VMError e){
        e.what();
        std::cout << "ERRCOMMAND: ";
        vmr.disasm();
        std::cout << "Full of program:" << std::endl;
        for(int i = 0;i < vmr.pc.Command_List.size();i=i+1){
            vmr.disasm(i);
        }
    }
    return 0;
}