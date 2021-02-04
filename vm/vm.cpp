#include "core.cpp"
using namespace std;

int main(){
    VMExec vme;
    vme.head.hash = 0x114514ff;
    vme.head.support_vm_build = 1;
    vme.head.code_label_count = 0;
    vme.head.code_length = 0;
    vme.head.a_msg.build = 0;
    
    return 0;
}