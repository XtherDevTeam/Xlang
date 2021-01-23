#include "core.cpp"
using namespace std;

int main(){
    VMExec vme;
    vme.head.hash = 0x114514ff;
    vme.head.support_vm_build = 1;
    vme.head.code_label_count = 0;
    vme.head.code_length = 0;
    vme.head.a_msg.build = 0;
    strcpy(vme.head.a_msg.ProgramName,"Test VM Execute"); // FIXME: There is a bug on clang compiler
    vme.cpool.pool = (char*)malloc(12);
    strcpy(vme.cpool.pool,"Hello,World"); // FIXME: same
    vme.cpool.size = 12;
    VMExec_Serialization("test.vme",vme);
    VMRuntime rt(vme);
    rt.Run();
    Content rc;
    rc.intc = 233;
    rt.stack_a.push(rc);
    
    rc = rt.stack_a.pop();
    cout << rc.intc;
    return 0;
}