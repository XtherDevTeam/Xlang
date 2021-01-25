#include "compiler/compiler.cpp"
using namespace std;

int main(){
    Runtime_Stack stk((char*)malloc(1024));
    Content tmp;
    tmp.intc = 114514;
    stk.push(tmp);
    stk.newFrame();
    tmp.intc = 1919810;
    stk.push(tmp);
    cout << stk.pop().intc << endl;
    stk.PopFrame();
    cout << stk.pop().intc << endl;
}
