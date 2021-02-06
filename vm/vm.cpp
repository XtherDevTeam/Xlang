#include "core.cpp"
 

int main(){
    std::string path;
    getline( std::cin,path);
    VMExec vme;
    LoadVMExec((char*)path.c_str(),&vme);
    VMRuntime vmr(vme);
    vmr.vm_rules["verbose"] = true;
    vmr.Run();
    DebugOutput(vmr);
    Memory_Watcher(vmr,0);
    return 0;
}