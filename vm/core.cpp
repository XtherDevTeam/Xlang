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

// 
typedef unsigned long int addr_t;

class VMError{
    public:
    std::string str;
    VMError(std::string msg){
        str = msg;
    }
    void what(){
        std::cerr << "\033[31m[VMERR]\033[0m " << str << "\n";
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
    write(fd,&vme.head,sizeof(VMExecHeader));

    write(fd,vme.label_array,sizeof(CodeLabel) * vme.head.code_label_count);
    write(fd,vme.code_array,sizeof(ByteCode) * vme.head.code_length);

    write(fd,&vme.cpool.size,8);
    write(fd,vme.cpool.pool,vme.cpool.size);
    close(fd);
}

// return an not closed file handle for constant pool init
int LoadVMExec(char* filename,VMExec& vme){
    int fd = open(filename,O_RDWR|O_CREAT,0777);
    //vme->cpool = (ConstantPool*)malloc(sizeof(ConstantPool));
    read(fd,&vme.head,sizeof(VMExecHeader));

    vme.label_array = (CodeLabel*)malloc(sizeof(CodeLabel) * vme.head.code_label_count);
    vme.code_array = (ByteCode*)malloc(sizeof(ByteCode) * vme.head.code_length);
    read(fd,vme.label_array,sizeof(CodeLabel) * vme.head.code_label_count);
    read(fd,vme.code_array,sizeof(ByteCode) * vme.head.code_length);

    read(fd,&vme.cpool.size,8);
    return fd;
}

struct heap_item_t{
    addr_t c[3];
    addr_t& operator[](int index){
        return c[index];
    }
};

class Runtime_Stack{
    std::string* base_memory = nullptr;
    public:
    long int fp,sp;

    Runtime_Stack(std::string& str){
        base_memory = &str;
        fp = 0,sp = 0;
    }
    Runtime_Stack(){
        fp = 0,sp = 0;
        base_memory = nullptr;
    }
    void push(Content c){
        char* ref = &(*base_memory)[base_memory->size() - 1 - fp - sp - 8 + 1];
        for(int i = 0;i < 8;i=i+1,ref++){
            (*ref) = c.chc[i];
        }
        sp += 8;
    }
    void push(char* chs,size_t size){
        char* ref = &(*base_memory)[base_memory->size() - 1 - fp - sp - size + 1];
        for(int i = 0;i < size;i=i+1,ref++){
            (*ref) = chs[i];
        }
        sp += size;
    }
    Content& pop(){
        Content* ref = (Content*)&(*base_memory)[base_memory->size() - 1 - fp - sp + 1];
        sp -= 8;
        return *ref;
    }
    char* pop(size_t size){
        char* ret = (char*)base_memory->data() + base_memory->size() - 1 - fp - sp + 1;
        sp -= size;
        return ret;
    }
    void save(){
        Content s;
        s.intc = fp;
        push(s);
        fp = sp;
        sp = 0;
    }
    void pop_frame(){
        pop(sp);
        sp = fp;
        fp = pop().intc;
        std::cout << "result:" << fp << " , " << sp << std::endl;
    }
    void output(std::ostream& out){
        for(int i = 1023;i >= 1023 - 16;i--){
            out << (int)(*base_memory)[i] << std::setw(1) << " ";
        }
    }
};

// 暂时不需要实现这玩意
class Runtime_Heap{
    std::vector<heap_item_t> list;
    std::string* base_memory;
    size_t start,top;
    size_t FindFreeBlock(size_t need){
        size_t bt = 0;
        bool hasSqareBlock = false;
        if(list.empty()) return LONG_MAX;
        for(size_t i = 0;i < list.size();i++){
            if(list[i][0] == false){
                // not-using 
                if(list[i][1] == need){
                    list[i][0] = true;
                    return i;
                }
                else if(list[i][1] > need && list[i][1] < list[bt][1]){
                    bt = i;
                    hasSqareBlock = true;
                }
            }
        }
        // not-equal
        if(hasSqareBlock){
            heap_item_t h;
            h[0] = false;
            h[1] = list[bt][1] - need;
            h[2] = list[bt][2] + list[bt][1];
            list.push_back(h);
            return list.size() - 1;
        }
        else{
            return LONG_MAX;
        }
    }
    public:
    Runtime_Heap(std::string& base ,size_t start){
        this->base_memory = &base;
        this->start = start;
    }
    Runtime_Heap(){}
    // return Id
    size_t InsertToHeap(Content s){
        size_t ret;
        if((ret = FindFreeBlock(8)) != LONG_MAX);
        else{
            heap_item_t h;
            h[0] = true;
            h[1] = 8;
            h[2] = top;
            top += 8;
            list.push_back(h);
            ret = list.size() - 1;
        }
        char* ref = (char*)base_memory->data() + list[ret][2];
        for(int i = 0;i < 8;i++,ref++){
            (*ref) = s.chc[i];
        }
        return ret;
    }
    size_t InsertToHeap(std::string& s){
        size_t ret;
        if((ret = FindFreeBlock(s.size())) != LONG_MAX);
        else{
            heap_item_t h;
            h[0] = true;
            h[1] = s.size();
            h[2] = top;
            top += s.size();
            list.push_back(h);
            ret = list.size() - 1;
        }
        char* ref = (char*)base_memory->data() + list[ret][2];
        for(int i = 0;i < s.size();i++,ref++){
            (*ref) = s[i];
        }
        return ret;
    }
    void unlink(size_t id){
        list[id][0] = false;
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
std::string COMMAND_MAP[] = {
    "mov","mov_m","push","pop","save","pop_frame",
    "add","sub","mul","div",
    "equ","maxeq","mineq","max","min",
    "goto","gt","gf","call",
    "exit","ret"
};
std::map<std::string,long> realmap;
class PC_Register{
    public:
    std::string* basestr;
    ByteCode* offset;
    long current_offset;
    PC_Register(){};
    PC_Register(std::string& basestr,void* start){
        this->basestr = &basestr;
        offset = (ByteCode*)start;
    }
    void operator++(int x){
        while(true){
            offset++;
            if(offset->opid == Command) break;
        }
    }
    void operator--(int x){
        while(true){
            offset--;
            if(offset->opid == Command) break;
        }
    }
    void operator+=(long c){
        //long  = 0;
        if(c > 0) for(int i = 0;i < c;i++) (*this)++;
        if(c < 0) for(int i = 0;i < 0-c;i++) (*this)--;
    }
    void operator=(ByteCode* pos){
        offset = pos;
    }
};

struct Device{
    char device_name[32];
    void* (device_request)(void* linked_vmruntime,char req_motd,addr_t args);
    void* (different_request)(void* linked_vmruntime,addr_t args);
};

struct DevicePackage{
    std::vector<int> require_int_num;
    char device_name[32];
};

Content public_temp_place;
class VMRuntime{
    VMExec vme;
    std::string allocated_memory;
    public:
    int              vme_fd;
    char*            malloc_place;
    size_t           _Alloc_Size;
    bool             regflag;
    PC_Register      pc;
    std::map<std::string,bool> vm_rules;
    Content          regs[32];
    Runtime_Heap     heap;
    Runtime_Stack    stack_a;
    ByteCode*        program;
    char*            constant_pool;

    Content& getRegRefernce(int rid){
        return regs[rid];
    }
    void Bind_VMExec(VMExec vme,int unread_rid){
        this->vme = vme;
        this->vme_fd = unread_rid;
    }
    void disasm(std::ostream &out = std::cout){
        out << COMMAND_MAP[pc.offset->c.intc] << " ";
        if((pc.offset+1)->opid == Command) {std::cout << ";\n";return;}
        for(auto i = pc.offset + 1;(i)->opid != Command;i=i+1){
            if(i->opid == Number) out << i->c.intc;
            if(i->opid == NormalRegister) out << "reg" << i->c.intc;
            if(i->opid == UnusualRegister) out << "ureg" << i->c.intc;
            if(i->opid == Address) out << "[" << i->c.intc << "]";
            if(i->opid == Address_Register) out << "[reg" << i->c.intc << "]";
            out << ",";
        }
        out << "\b;" << std::endl;
    }
    char* GetMemberAddress(ByteCode& a){
        if(a.opid == UnusualRegister){
            if(a.c.intc == 0) return (char*)&stack_a.fp;
            if(a.c.intc == 1) return (char*)&stack_a.sp;
            if(a.c.intc == 2) return (char*)pc.offset;
            if(a.c.intc == 3) return nullptr;
        }
        else if(a.opid == NormalRegister)  return (char*)&getRegRefernce(a.c.intc);
        else if(a.opid == Address) return malloc_place + a.c.intc;
        else if(a.opid == Address_Register) return malloc_place + getRegRefernce(a.c.intc).intc;
        else return nullptr;
    }
    void StartVMProc(){
        for(CodeLabel* i = vme.label_array;i < vme.label_array+vme.head.code_label_count;i++){
            if(std::string(i->label_n) == "_vmstart") pc.offset = program + i->start;
            //if(strcmp(i->label_n,"_main_int")) pc.offset = program + i->start;
        }
        while(pc.offset->c.intc != realmap["exit"]){
            disasm();
            if(pc.offset->c.intc == realmap["mov"]){
                // Normal move command, only support 8 byte
                char* _dest;
                if((_dest = GetMemberAddress(*(pc.offset + 1))) == nullptr) throw VMError("move dest must be a currently place");
                if(GetMemberAddress(*(pc.offset + 2)) == nullptr){
                    // 实数
                    if((pc.offset+2)->opid == Number) for(int i = 0;i < 8;i=i+1) *(_dest + i) = (pc.offset+2)->c.chc[i];
                    else if((pc.offset+2)->c.intc == 3) ((Content*)_dest)->intc = allocated_memory.size() - 1;
                    else throw VMError("Invalid Move Command");
                }else{
                    char* _Src = GetMemberAddress(*(pc.offset + 2));
                    for(int i = 0;i < 8;i++) *(_dest + i) = *(_Src + i);
                }
            }else if(pc.offset->c.intc == realmap["mov_m"]){
                char* _dest;long size = (pc.offset+3)->c.intc;
                if((_dest = GetMemberAddress(*(pc.offset + 1))) == nullptr) throw VMError("move dest must be a currently place");
                if(GetMemberAddress(*(pc.offset + 2)) == nullptr){
                    // 实数
                    if((pc.offset+2)->opid == Number) for(int i = 0;i < 8;i=i+1) *(_dest + i) = (pc.offset+2)->c.chc[i];
                    else if((pc.offset+2)->c.intc == 3) ((Content*)_dest)->intc = allocated_memory.size() - 1;
                    else throw VMError("Invalid Move Command");
                }else{
                    char* _Src = GetMemberAddress(*(pc.offset + 2));
                    for(long int i = 0;i < size;i++) *(_dest + size - 1 - i) = *(_Src + size - 1 - i);
                }
            }else if(pc.offset->c.intc == realmap["push"]){
                char* dest = GetMemberAddress(*(pc.offset+1));long size = (pc.offset+2)->c.intc;
                if(dest == nullptr) throw VMError("push:Unknown data address");
                stack_a.push(dest,size);
            }else if(pc.offset->c.intc == realmap["pop"]){
                char* dest = GetMemberAddress(*(pc.offset+1));long size = (pc.offset+2)->c.intc;
                if(dest == nullptr) throw VMError("pop:Unknown data address");
                char* _Src = stack_a.pop(size);
                for(long int i = 0;i < size;i++) *(dest + size - 1 - i) = *(_Src + size - 1 - i);
            }else if(pc.offset->c.intc == realmap["save"]){
                Content s;
                s.ptrc = pc.offset;
                stack_a.push(s);
                for(int i = 0;i < 32;i++){
                    stack_a.push(regs[i]);
                }
                stack_a.push((char*)&regflag,1);
                stack_a.save();
            }else if(pc.offset->c.intc == realmap["pop_frame"]){
                stack_a.pop_frame();
                regflag = *(stack_a.pop(1));
                for(int i = 31;i >= 0;i--){
                    regs[i] = stack_a.pop();
                }
                public_temp_place = stack_a.pop();
            }else if(pc.offset->c.intc == realmap["add"] || pc.offset->c.intc == realmap["sub"] || pc.offset->c.intc == realmap["mul"] || pc.offset->c.intc == realmap["div"]){
                Content* _dest = (Content*)GetMemberAddress(*(pc.offset+1)),*_Src = (Content*)GetMemberAddress(*(pc.offset+2));
                if(_dest == nullptr) throw VMError("add:_dest doesn't a place");
                if(_Src != nullptr){
                    if(pc.offset->c.intc == realmap["add"])      _dest->intc = _dest->intc + _Src->intc;
                    else if(pc.offset->c.intc == realmap["sub"]) _dest->intc = _dest->intc - _Src->intc;
                    else if(pc.offset->c.intc == realmap["mul"]) _dest->intc = _dest->intc * _Src->intc;
                    else if(pc.offset->c.intc == realmap["div"]) _dest->intc = _dest->intc / _Src->intc;
                }else{
                    Content s = (pc.offset + 2)->c;
                    if((pc.offset+2)->c.intc == 3 && (pc.offset+2)->opid == UnusualRegister) s.intc = allocated_memory.size() - 1;
                    if(pc.offset->c.intc == realmap["add"]) _dest->intc += s.intc;
                    if(pc.offset->c.intc == realmap["sub"]) _dest->intc -= s.intc;
                    if(pc.offset->c.intc == realmap["mul"]) _dest->intc *= s.intc;
                    if(pc.offset->c.intc == realmap["div"]) _dest->intc /= s.intc;
                }
            }else if(pc.offset->c.intc == realmap["equ"] || pc.offset->c.intc == realmap["maxeq"] || pc.offset->c.intc == realmap["mineq"] ||
                     pc.offset->c.intc == realmap["max"] || pc.offset->c.intc == realmap["min"]
                    ){
                Content _dest,_src;
                Content* tmp = (Content*)GetMemberAddress(*(pc.offset+1));
                if(tmp != nullptr) _dest = *tmp;
                else _dest.intc = (pc.offset+1)->c.intc;
                tmp = (Content*)GetMemberAddress(*(pc.offset+2));
                if(tmp != nullptr) _src = *tmp;
                else _src.intc = (pc.offset+2)->c.intc;
                if(pc.offset->c.intc == realmap["equ"])   regflag = (_dest.intc == _src.intc);
                if(pc.offset->c.intc == realmap["maxeq"]) regflag = (_dest.intc >= _src.intc);
                if(pc.offset->c.intc == realmap["mineq"]) regflag = (_dest.intc <= _src.intc);
                if(pc.offset->c.intc == realmap["max"])   regflag = (_dest.intc > _src.intc);
                if(pc.offset->c.intc == realmap["min"])   regflag = (_dest.intc < _src.intc);
            }else if(pc.offset->c.intc == realmap["goto"] || pc.offset->c.intc == realmap["gt"] || pc.offset->c.intc == realmap["gf"]){
                Content s;
                if(GetMemberAddress(*(pc.offset + 1)) != nullptr) s = *(Content*)GetMemberAddress(*(pc.offset+1));
                else s = (pc.offset+1)->c;
                if(pc.offset->c.intc == realmap["goto"]) {pc += s.intc;continue;}
                else if(pc.offset->c.intc == realmap["gt"] && regflag == 1) {pc += s.intc;continue;}
                else if(pc.offset->c.intc == realmap["gf"] && regflag == 0) {pc += s.intc;continue;}
            }else if(pc.offset->c.intc == realmap["call"]){
                //long i=;
                pc = program + vme.label_array[(pc.offset+1)->c.intc].start;
                continue;
            }else if(pc.offset->c.intc == realmap["ret"]){
                char* _Src = GetMemberAddress(*(pc.offset+1));
                stack_a.pop_frame();
                regflag = stack_a.pop(1);
                for(int i = 31;i >= 0;i--){
                    regs[i] = stack_a.pop();
                }
                public_temp_place = stack_a.pop();
                long size = (pc.offset+2)->c.intc;
                stack_a.push(_Src,size);
                pc.offset = (ByteCode*)public_temp_place.ptrc;
                //continue;
                stack_a.output(std::cout);
                pc++;
            }else if(pc.offset->c.intc == realmap["exit"]) return;
            pc++;
        }
    }
    void Run(addr_t _AllocSize = 0){
        if(_AllocSize == 0){
            // allocate by program
            _AllocSize = vme.head.code_length * 16 * 5;
            _Alloc_Size = _AllocSize;
        }
        allocated_memory.resize(_Alloc_Size);
        malloc_place = (char*)allocated_memory.data();
        memset(malloc_place,0,_Alloc_Size);
        char* memtop = malloc_place;

        constant_pool = (char*)&memtop;
        read(vme_fd,malloc_place,vme.cpool.size);
        memtop += vme.cpool.size;

        program = (ByteCode*)memtop;
        memcpy(program,vme.code_array,vme.head.code_length * 16);
        memtop += vme.head.code_length * 16;

        stack_a = Runtime_Stack(allocated_memory);
        heap = Runtime_Heap(allocated_memory,vme.cpool.size+1);
        if(vm_rules["verbose"] == true){
            printf("Xtime VM Core[1.0.01]\nStarting...\n");
        }
        for(int i = 0;i < 21;i++){
            //std::cout << "COMMAND:" << COMMAND_MAP[i] << std::endl;
            realmap[COMMAND_MAP[i]] = i;
        }
        StartVMProc();
    }
    VMRuntime(VMExec vme,int unread_rid){
        heap = Runtime_Heap();
        regflag = 0;
        memset(&regs,0,32*sizeof(Content));
        Bind_VMExec(vme,unread_rid);
        pc = PC_Register(allocated_memory,(void*)program);
    }
};

void disasm(ByteCode* program,size_t length){
    for(int i = 0;i < length;i++){
        if((program+i)->opid == Command) std::cout << std::endl << COMMAND_MAP[(program+i)->c.intc] << " ";
        if((program+i)->opid == Number)  std::cout << (program + i)->c.intc << ",";
        if((program+i)->opid == NormalRegister) std::cout << "reg" << (program + i)->c.intc << ",";
        if((program+i)->opid == Address_Register) std::cout << "[" << "reg" << (program + i)->c.intc << "],";
        if((program+i)->opid == UnusualRegister) std::cout << "ureg" << (program + i)->c.intc << ",";
        if((program+i)->opid == Address) std::cout << "[" << (program + i)->c.intc << "],";
    }
}

void DebugOutput(VMRuntime rt, std::ostream &out = std::cout){
    out << "==========================[Debug Output]==========================\n";
    for(int i = 0;i < 32;i=i+1){
        if(i < 10) out << "reg" << i << " : " << rt.regs[i].intc << " ";
        else out << "reg" << i << ": " << rt.regs[i].intc << " ";
        if(i % 7 == 0 && i != 0) out <<  std::endl; 
    }
    out << "\n";
    out << "REGFLAG:" << rt.regflag << " PC:" << rt.pc.offset <<  std::endl;
    out << "==========================[EndOf Output]==========================\n";
}

void Memory_Watcher(VMRuntime rt,long v,std::ostream &out = std::cout){
    Content *c = (Content*)(&rt.malloc_place[v]);
    out << "Memory Watcher=>" << v << "\n int val=>" << c->intc << "\n Charter Val=>" << std::string(c->chc,8) << "\n";
}