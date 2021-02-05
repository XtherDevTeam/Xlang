#include "core.cpp"
using namespace std;

int main(){
    string path;
    getline(cin,path);
    VMExec vme;
    LoadVMExec((char*)path.c_str(),&vme);
    VMRuntime vmr(vme);
    vmr.vm_rules["verbose"] = true;
    vmr.Run();
    DebugOutput(vmr);
    return 0;
}