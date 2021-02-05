/*
  git config --global user.email "3134714226@qq.com"
  git config --global user.name "Xlang_Xiaokang00010"
*/
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include "../compiler/basic_type.cpp"
#include "const.cpp"

using namespace std;
typedef unsigned long int addr_t;

class VMError{
    public:
    string str;
    VMError(string msg){
        str = msg;
    }
    void what(){
        cerr << "\033[31m[VMERR]\033[0m " << str << "\n";
    }
};

struct ByteCode{
    char opid;
    Content c;
};

struct CodeLabel{
    char label_n[32]; // Label name
    int label_id;   // for goto command
    long int start; // start position
};

struct AuthorMessage{
    char AuthorName[32];
    char ProgramName[32];
    char major,minjor,build;
};

struct VMExecHeader{
    long int hash; // default is 0x114514ff
    short support_vm_build; // support vm build version
    long int code_label_count; // if this is zero , throw Error
    size_t code_length;
    AuthorMessage a_msg;
};

int s=sizeof(CodeLabel);

struct ConstantPool{
    addr_t size,count;
    size_t* items;
    char* pool;
};

struct VMExec{
    VMExecHeader head;
    CodeLabel* label_array;
    ByteCode* code_array;
    ConstantPool cpool;
};

void VMExec_Serialization(char* filename,VMExec vme){
    int fd = open(filename,O_RDWR|O_CREAT,0777);
    if(fd == -1) perror("File open error!");
    write(fd,&vme.head,sizeof(vme.head));
    write(fd,vme.label_array,vme.head.code_label_count * 48);
    write(fd,vme.code_array,vme.head.code_length * 16);
    write(fd,&vme.cpool.count,8);
    write(fd,&vme.cpool.size,8);
    write(fd,vme.cpool.items,vme.cpool.count * 8);
    write(fd,vme.cpool.pool,vme.cpool.size);
}

int LoadVMExec(char* filename,VMExec* vme){
    int fd = open(filename,O_RDWR|O_CREAT,0777);
    if(read(fd,&vme->head,sizeof(VMExecHeader)) == -1)  return VM_FAILED_OPEN;
    if(vme->head.hash != 0x114514ff) return VM_INVALID_HASH;
    vme->label_array = (CodeLabel*)malloc(vme->head.code_label_count * 48);
    if(vme->label_array == NULL) return VM_FAILED_OPEN;
    else if(read(fd,vme->label_array,vme->head.code_label_count * 48) == -1) return VM_FAILED_OPEN;
    vme->code_array = (ByteCode*)malloc(vme->head.code_length * 16);
    if(vme->code_array == NULL) return VM_FAILED_OPEN;
    else if(read(fd,vme->code_array,vme->head.code_length * 16) == -1) return VM_FAILED_OPEN;
    if(read(fd,&vme->cpool,sizeof(addr_t) * 8) == -1) return VM_FAILED_OPEN;
    vme->cpool.items = (size_t*)malloc(vme->cpool.count * 8);
    vme->cpool.pool = (char*)malloc(vme->cpool.size);
    read(fd,&vme->cpool.items,vme->cpool.count * 8);
    read(fd,&vme->cpool.pool,vme->cpool.size);

    return VM_SUCCES_LOADED;
}

struct heap_item_t{
    addr_t c[3];
    addr_t& operator[](int index){
        return c[index];
    }
};

class Runtime_Heap{
    public:
    char* allocate_addr;
    addr_t top = 0;
    vector<heap_item_t> heap_item; // {IsUsing, start, end}
    Runtime_Heap(size_t alloc){
        top = 0;
        allocate_addr = (char*)malloc(alloc);
        if(allocate_addr == NULL) printf("!!![ERROR]!!! Cannot allocate memory for heap!");
    }
    Runtime_Heap(){
        allocate_addr = NULL;
        top = 0;
    }
    // Find Non-using block
    int findNonUsing(int _Size){
        int spare_pos = -1; // if this_item.size > _Size , then put that here
        if(heap_item.empty()) return INT_MAX;
        for(int i = 0;i < heap_item.size();i++){
            // Find Non-using block
            if(heap_item[i][0] == 0){
                if(heap_item[i][2] - heap_item[i][1] == _Size) return i; // got equal size block
                if(heap_item[i][2] - heap_item[i][1] > _Size){
                    if(spare_pos == -1) spare_pos = i;
                    // 尽可能拆分较小的块
                    else if( (heap_item[i][2] - heap_item[i][1]) < (heap_item[spare_pos][2] - heap_item[spare_pos][1]) ) spare_pos = i;
                }
            }
        }
        // no equal block find
        if(spare_pos != -1){
            heap_item[spare_pos][0] = 1;
            addr_t tmp = heap_item[spare_pos][2];
            heap_item[spare_pos][2] = heap_item[spare_pos][1] + _Size;
            heap_item_t hit;
            // {0,heap_item[spare_pos][1] + _Size,tmp}
            hit[0] = 0;
            hit[1] = heap_item[spare_pos][1] + _Size;
            hit[2] = tmp;
            heap_item.push_back(hit);
            return spare_pos;
        }
        return INT_MAX;
    }

    // Basic int64 insert function
    int InsertToHeap(Content c){
        int tmp = 0;
        if((tmp = findNonUsing(8)) != INT_MAX){
            memcpy(allocate_addr + heap_item[tmp][1],c.chc,8);
            return tmp;
        }
        memcpy(allocate_addr+top,c.chc,8);
        heap_item_t hit;
        //hit.c = {1,top,top+8};
        hit[0] = 1;
        hit[1] = top;
        hit[2] = top+8;
        heap_item.push_back(hit);
        top += 8;
        return heap_item.size() - 1; 
    }
    // String Insert Function
    int InsertToHeap(char* _Src,addr_t sz){
        int tmp = 0;
        if((tmp = findNonUsing(sz)) != INT_MAX){
            memcpy(allocate_addr + heap_item[tmp][1],_Src,sz);
            return tmp;
        }
        memcpy(allocate_addr + top,_Src,sz);
        heap_item_t hit;
        //hit.c = {1,top,top+sz};
        hit[0] = 1;
        hit[1] = top;
        hit[2] = top+sz;
        heap_item.push_back(hit);
        top += sz;
        return heap_item.size() - 1; 
    }
    // Free
    void free_item(int pos){
        if(pos >= heap_item.size()){
            printf("!!![ERROR]!!! Position %d not exist!\n",pos);
        }
        heap_item[pos][0] = 0;
    }
};

class Runtime_Stack{
    char* allocate_addr;
    public:
    addr_t fp,sp;
    Runtime_Stack(char* aaddr){
        allocate_addr = aaddr;
        fp = 0;
        sp = 0;
    }
    Runtime_Stack(){
        fp = 0;
        sp = 0;
    }
    // Normal push function for 8 bytes
    void push(Content s){
        memcpy(allocate_addr - fp - sp - 8,s.chc,8);
        sp+=8;
    }
    // String push function
    void push(char* s,addr_t length){
        memcpy(allocate_addr-fp-sp-length,s,length);
        sp += length;
    }
    // Normal pop function
    Content pop(){
        Content s;
        //cout << "sp:" << sp << endl;
        sp -= 8;
        memcpy(s.chc,allocate_addr - fp - sp - 8,8); // WARN: 虽然我也不知道为什么这里还要减一次sp，但！是！鸽子飞起来就好！
        return s;
    }
    char* pop(addr_t size){
        char* ret = (char*)malloc(size);
        sp -= size;
        memcpy(ret,allocate_addr - fp - sp + size,size); // WARN: 同上
        return ret;
    }
    void newFrame(){
        Content c;
        c.intc = fp;
        push(c);
        c.intc = sp;
        push(c);
        fp = sp;sp = 0;
    }
    void PopFrame(){
        //cout << "sp:" << fp << "," << sp << endl;
        free(pop(sp));
        sp = pop().intc;
        fp = pop().intc;
        //cout << "pop result:" << fp << "," << sp << endl;
    }
};

enum opid_list{
    UnusualRegister = 1,
    NormalRegister  = 2,
    Command         = 3,
    Number          = 4,
    Address         = 5,
    Address_Register= 6,
} opid_kind;
string COMMAND_MAP[] = {
    "mov","mov_m","push","pop","save","pop_frame",
    "add","sub","mul","div",
    "equ","maxeq","mineq","max","min",
    "goto","gt","gf",
    "exit","ret"
};

class PC_Register{
    vector<size_t> Command_List;
    public:
    long current_offset,current_command;
    PC_Register(){};
    PC_Register(ByteCode* byc,size_t bytecode_length){
        for(int i = 0;i < bytecode_length;i=i+1){
            if((opid_list)byc[i].opid == Command){
                Command_List.push_back(i);
            }
        }
        current_offset = Command_List[0];
        current_command = 0;
    }
    void operator+=(int offset){
        current_command += offset;
        current_offset = Command_List[current_command];
    }
    void operator++(int s){
        current_command++;
        current_offset = Command_List[current_command];
    }
    void operator=(int current){
        current_command = current;
        current_offset = Command_List[current_command];
    }
    void UpdateOffset(){
        current_offset = Command_List[current_command];
    }
};

struct Device{
    char device_name[32];
    void* (device_request)(void* linked_vmruntime,char req_motd,addr_t args);
    void* (different_request)(void* linked_vmruntime,addr_t args);
};

struct DevicePackage{
    vector<int> require_int_num;
    char device_name[32];
};

class VMRuntime{
    VMExec vme;
    char* malloc_place;
    public:
    size_t _Alloc_Size;
    bool             regflag;
    PC_Register      pc;
    map<string,bool> vm_rules;
    Content          regs[32];
    Runtime_Heap     heap;
    Runtime_Stack    stack_a;
    ByteCode*        program;
    char*            constant_pool;

    Content& getRegRefernce(int rid){
        return regs[rid];
    }
    void Bind_VMExec(VMExec vme){
        this->vme = vme;
    }
    void StartVMProc(){
        for(CodeLabel* i = vme.label_array;i < vme.label_array+vme.head.code_label_count;i++){
            if(strcmp(i->label_n,"_vmstart")) pc = i->start;
            if(strcmp(i->label_n,"_main_int")) pc = i->start;
        }
        while(true){
            if(COMMAND_MAP[program[pc.current_offset].c.intc] == "mov"){
                Content *r = NULL;
                if(program[pc.current_offset+1].opid == NormalRegister) r = &getRegRefernce(program[pc.current_offset+1].c.intc);
                else if(program[pc.current_offset+1].opid == UnusualRegister){
                    if(program[pc.current_offset+1].c.intc == 2) r = (Content*)&pc.current_offset;
                    else throw VMError("Write-protected register");
                }
                else throw VMError("Invalid move action");
                if(program[pc.current_offset+2].opid == NormalRegister) *r = getRegRefernce(program[pc.current_offset+2].c.intc);
                else if(program[pc.current_offset+2].opid == Number) r->intc = program[pc.current_offset+2].c.intc;
                else if(program[pc.current_offset+2].opid == UnusualRegister){
                    if(program[pc.current_offset+2].c.intc == 0) /*fp*/ r->intc = stack_a.fp;
                    else if(program[pc.current_offset+2].c.intc == 1) /*sp*/ r->intc = stack_a.sp;
                    else if(program[pc.current_offset+2].c.intc == 2) /*pc*/ r->intc = pc.current_offset;
                    else if(program[pc.current_offset+2].c.intc == 3) /*sb*/ r->intc = _Alloc_Size - 1;
                    else throw VMError("Invalid Unusual Register Id");
                }
                else throw VMError("Invalid Right Value");
                if(program[pc.current_offset+1].opid == UnusualRegister && program[pc.current_offset+1].c.intc == 2){pc.UpdateOffset();continue;}
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "mov_m"){
                long size = program[pc.current_offset+3].c.intc;
                char *toWrite = NULL, *_Src = NULL;
                if(program[pc.current_offset+1].opid == NormalRegister) toWrite = (char*)&getRegRefernce(program[pc.current_offset+1].c.intc);
                else if(program[pc.current_offset+1].opid == Address) toWrite = (char*)malloc_place + program[pc.current_offset+1].c.intc;
                else if(program[pc.current_offset+1].opid == Address_Register) toWrite = (char*)malloc_place + getRegRefernce(program[pc.current_offset+1].c.intc).intc;
                else throw VMError("Bad Left Value");
                if(program[pc.current_offset+2].opid == NormalRegister) _Src = (char*)&getRegRefernce(program[pc.current_offset+2].c.intc);
                else if(program[pc.current_offset+2].opid == Address) _Src = (char*)malloc_place + program[pc.current_offset+2].c.intc;
                else if(program[pc.current_offset+2].opid == Address_Register) _Src = (char*)malloc_place + getRegRefernce(program[pc.current_offset+2].c.intc).intc;
                else if(program[pc.current_offset+2].opid == UnusualRegister){
                    if(program[pc.current_offset+2].c.intc == 0) /*fp*/ _Src = (char*)&stack_a.fp;
                    else if(program[pc.current_offset+2].c.intc == 1) /*sp*/ _Src = (char*)&stack_a.sp;
                    else if(program[pc.current_offset+2].c.intc == 2) /*pc*/ _Src = (char*)&pc.current_offset;
                    else if(program[pc.current_offset+2].c.intc == 3) /*sb*/ {
                        Content s;
                        s.intc =_Alloc_Size - 1;
                        _Src = (char*)string(s.chc,8).c_str();
                    }
                    else throw VMError("Invalid Unusual Register Id");
                }
                else if(program[pc.current_offset+2].opid == Number){
                    Content s;
                    s.intc = program[pc.current_offset+2].c.intc;
                    _Src = (char*)string(s.chc,8).c_str();
                }
                else throw VMError("Bad Right Value");
                memcpy(toWrite,_Src,size);
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "push"){
                long size = program[pc.current_offset+2].c.intc;
                char *_Src = NULL;
                if(program[pc.current_offset+1].opid == NormalRegister) _Src = (char*)&getRegRefernce(program[pc.current_offset+1].c.intc);
                else if(program[pc.current_offset+1].opid == Address) _Src = (char*)malloc_place + program[pc.current_offset+1].c.intc;
                else if(program[pc.current_offset+1].opid == Address_Register) _Src = (char*)malloc_place + getRegRefernce(program[pc.current_offset+1].c.intc).intc;
                else if(program[pc.current_offset+1].opid == UnusualRegister){
                    if(program[pc.current_offset+1].c.intc == 0) /*fp*/ _Src = (char*)&stack_a.fp;
                    else if(program[pc.current_offset+1].c.intc == 1) /*sp*/ _Src = (char*)&stack_a.sp;
                    else if(program[pc.current_offset+1].c.intc == 2) /*pc*/ _Src = (char*)&pc.current_offset;
                    else if(program[pc.current_offset+1].c.intc == 3) /*sb*/ {
                        Content s;
                        s.intc =_Alloc_Size - 1;
                        _Src = (char*)string(s.chc,8).c_str();
                    }
                    else throw VMError("Invalid Unusual Register Id");
                }
                else if(program[pc.current_offset+1].opid == Number){
                    Content s;
                    s.intc = program[pc.current_offset+1].c.intc;
                    _Src = (char*)string(s.chc,8).c_str();
                }
                else throw VMError("Bad Value");
                stack_a.push(_Src,size);
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "pop"){
                Content* regobj;
                if(program[pc.current_offset+1].opid == NormalRegister) regobj = &getRegRefernce(program[pc.current_offset+1].c.intc);
                else throw VMError("Only support pop to register now.");
                memcpy(regobj,stack_a.pop(8),8);
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "save"){
                stack_a.push((char*)&regs,sizeof(regs));
                stack_a.push((char*)&pc,sizeof(PC_Register));
                stack_a.push((char*)&regflag,1);
                stack_a.newFrame();
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "pop_frame"){
                stack_a.PopFrame();
                memcpy(&regflag,stack_a.pop(1),1);
                memcpy((char*)&pc,stack_a.pop(sizeof(PC_Register)),sizeof(PC_Register));
                memcpy((char*)&regs,stack_a.pop(sizeof(regs)),sizeof(regs));
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "add" || COMMAND_MAP[program[pc.current_offset].c.intc] == "sub" || COMMAND_MAP[program[pc.current_offset].c.intc] == "mul" || COMMAND_MAP[program[pc.current_offset].c.intc] == "div"){
                Content *_lhs,*_rhs;
                if(program[pc.current_offset+1].opid == NormalRegister) _lhs = (Content*)&getRegRefernce(program[pc.current_offset+1].c.intc);
                else if(program[pc.current_offset+1].opid == Address) _lhs = (Content*)malloc_place + program[pc.current_offset+1].c.intc;
                else if(program[pc.current_offset+1].opid == Address_Register) _lhs = (Content*)malloc_place + getRegRefernce(program[pc.current_offset+1].c.intc).intc;
                else throw VMError("Bad Left Value");
                if(program[pc.current_offset+2].opid == NormalRegister) _rhs = (Content*)&getRegRefernce(program[pc.current_offset+2].c.intc);
                else if(program[pc.current_offset+2].opid == Address) _rhs = (Content*)malloc_place + program[pc.current_offset+2].c.intc;
                else if(program[pc.current_offset+2].opid == Address_Register) _rhs = (Content*)malloc_place + getRegRefernce(program[pc.current_offset+2].c.intc).intc;
                else if(program[pc.current_offset+2].opid == UnusualRegister){
                    if(program[pc.current_offset+2].c.intc == 0) /*fp*/ _rhs = (Content*)&stack_a.fp;
                    else if(program[pc.current_offset+2].c.intc == 1) /*sp*/ _rhs = (Content*)&stack_a.sp;
                    else if(program[pc.current_offset+2].c.intc == 2) /*pc*/ _rhs = (Content*)&pc.current_offset;
                    else if(program[pc.current_offset+2].c.intc == 3) /*sb*/ {
                        Content s;
                        s.intc =_Alloc_Size - 1;
                        _rhs = (Content*)string(s.chc,8).c_str();
                    }
                    else throw VMError("Invalid Unusual Register Id");
                }
                else if(program[pc.current_offset+2].opid == Number){
                    Content s;
                    s.intc = program[pc.current_offset+2].c.intc;
                    _rhs = (Content*)string(s.chc,8).c_str();
                }
                else throw VMError("Bad Right Value");
                if(COMMAND_MAP[program[pc.current_offset].c.intc] == "add") _lhs->intc += _rhs->intc;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "sub") _lhs->intc -= _rhs->intc;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "mul") _lhs->intc *= _rhs->intc;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "div") _lhs->intc /= _rhs->intc;
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "equ" || COMMAND_MAP[program[pc.current_offset].c.intc] == "maxeq" || COMMAND_MAP[program[pc.current_offset].c.intc] == "mineq" || COMMAND_MAP[program[pc.current_offset].c.intc] == "max" || COMMAND_MAP[program[pc.current_offset].c.intc] == "min"){
                Content *_lhs,*_rhs;
                if(program[pc.current_offset+1].opid == NormalRegister) _lhs = (Content*)&getRegRefernce(program[pc.current_offset+1].c.intc);
                else if(program[pc.current_offset+1].opid == Address) _lhs = (Content*)malloc_place + program[pc.current_offset+1].c.intc;
                else if(program[pc.current_offset+1].opid == Address_Register) _lhs = (Content*)malloc_place + getRegRefernce(program[pc.current_offset+1].c.intc).intc;
                else throw VMError("Bad Left Value");
                if(program[pc.current_offset+2].opid == NormalRegister) _rhs = (Content*)&getRegRefernce(program[pc.current_offset+2].c.intc);
                else if(program[pc.current_offset+2].opid == Address) _rhs = (Content*)malloc_place + program[pc.current_offset+2].c.intc;
                else if(program[pc.current_offset+2].opid == Address_Register) _rhs = (Content*)malloc_place + getRegRefernce(program[pc.current_offset+2].c.intc).intc;
                else if(program[pc.current_offset+2].opid == UnusualRegister){
                    if(program[pc.current_offset+2].c.intc == 0) /*fp*/ _rhs = (Content*)&stack_a.fp;
                    else if(program[pc.current_offset+2].c.intc == 1) /*sp*/ _rhs = (Content*)&stack_a.sp;
                    else if(program[pc.current_offset+2].c.intc == 2) /*pc*/ _rhs = (Content*)&pc.current_offset;
                    else if(program[pc.current_offset+2].c.intc == 3) /*sb*/ {
                        Content s;
                        s.intc =_Alloc_Size - 1;
                        _rhs = (Content*)string(s.chc,8).c_str();
                    }
                    else throw VMError("Invalid Unusual Register Id");
                }
                else if(program[pc.current_offset+2].opid == Number){
                    Content s;
                    s.intc = program[pc.current_offset+2].c.intc;
                    _rhs = (Content*)string(s.chc,8).c_str();
                }
                else throw VMError("Bad Right Value");
                if(COMMAND_MAP[program[pc.current_offset].c.intc] == "equ" && _lhs->intc == _rhs->intc) regflag = 1;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "maxeq" && _rhs->intc >= _lhs->intc) regflag = 1;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "mineq" && _rhs->intc <= _lhs->intc) regflag = 1;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "max" && _rhs->intc > _lhs->intc) regflag = 1;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "min" && _rhs->intc < _lhs->intc) regflag = 1;
                else regflag = 0;
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "goto" || COMMAND_MAP[program[pc.current_offset].c.intc] == "gt" || COMMAND_MAP[program[pc.current_offset].c.intc] == "gf"){
                Content *_lhs;
                if(program[pc.current_offset+1].opid == NormalRegister) _lhs = (Content*)&getRegRefernce(program[pc.current_offset+1].c.intc);
                else if(program[pc.current_offset+1].opid == Address) _lhs = (Content*)malloc_place + program[pc.current_offset+1].c.intc;
                else if(program[pc.current_offset+1].opid == Address_Register) _lhs = (Content*)malloc_place + getRegRefernce(program[pc.current_offset+1].c.intc).intc;
                else if(program[pc.current_offset+1].opid == Number){
                    Content s;
                    s.intc = program[pc.current_offset+1].c.intc;
                    _lhs = (Content*)string(s.chc,8).c_str();
                }
                else throw VMError("Bad Value");
                if(COMMAND_MAP[program[pc.current_offset].c.intc] == "goto") pc+=_lhs->intc;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "gt" && regflag == true)  pc+=_lhs->intc;
                else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "gf" && regflag == false) pc+=_lhs->intc;
                continue; // 流程控制语句防止pc ++ 执行 
            }
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "exit") return;
            else if(COMMAND_MAP[program[pc.current_offset].c.intc] == "ret"){
                if(program[pc.current_offset+1].opid != NormalRegister) throw VMError("Only support return register value now");
                Content regval = getRegRefernce(program[pc.current_offset+1].c.intc);
                stack_a.PopFrame();
                memcpy(&regflag,stack_a.pop(1),1);
                memcpy((char*)&pc,stack_a.pop(sizeof(PC_Register)),sizeof(PC_Register));
                memcpy((char*)&regs,stack_a.pop(sizeof(regs)),sizeof(regs));
                stack_a.push(regval);
            }
            pc++;
        }
    }
    void Run(addr_t _AllocSize = 0){
        if(_AllocSize == 0){
            // allocate by program
            _AllocSize = vme.head.code_length * 16 * 5;
            _Alloc_Size = _AllocSize;
        }
        malloc_place = (char*)malloc(_AllocSize);
        char* memtop = malloc_place;

        constant_pool = (char*)memtop;
        memcpy(memtop,vme.cpool.pool,vme.cpool.size);
        memtop += vme.cpool.size;

        program = (ByteCode*)memtop;
        memcpy(program,vme.code_array,vme.head.code_length * 16);
        memtop += vme.head.code_length * 16;

        heap.allocate_addr = (char*) memtop;
        stack_a = Runtime_Stack(malloc_place + _AllocSize - 1);
        if(vm_rules["verbose"] == true){
            printf("Xtime VM Core[1.0.01]\nStarting...\n");
        }
        StartVMProc();
    }
    VMRuntime(VMExec vme){
        regflag = 0;
        memset(&regs,0,32*sizeof(Content));
        Bind_VMExec(vme);
        pc = PC_Register(vme.code_array,vme.head.code_length);
    }
};

void DebugOutput(VMRuntime rt,ostream &out = cout){
    out << "==========================[Debug Output]==========================\n";
    for(int i = 0;i < 32;i=i+1){
        if(i < 10) out << "reg" << i << " : " << rt.regs[i].intc << " ";
        else out << "reg" << i << ": " << rt.regs[i].intc << " ";
        if(i % 7 == 0 && i != 0) out << endl; 
    }
    out << "\n";
    out << "REGFLAG:" << rt.regflag << " PC:" << rt.pc.current_command << endl;
    out << "==========================[EndOf Output]==========================\n";
}