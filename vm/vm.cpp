#include "core.cpp"

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
    Memory_Watcher(vmr,0);
    /*}catch(VMError e){
        e.what();
        std::cout << "ERRCOMMAND: ";
        vmr.disasm();
    }*/
    return 0;
}