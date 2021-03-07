/*
  git config --global user.email "3134714226@qq.com"
  git config --global user.name "Xlang_Xiaokang00010"
*/
#pragma once
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include "../compiler/basic_type.cpp"
#include "../lib/XVMDK/dlfcn.h"
#include "const.cpp"

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
    int label_id;   // for jmp command
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

void VMExec_Serialization(const char* filename,VMExec vme){
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
    public:
    int AllocSize;
    char* base_memory = nullptr;
    long int fp,sp;

    Runtime_Stack(std::string& str){
        base_memory = (char*)str.data();
        AllocSize = str.size();
        fp = 0,sp = 0;
    }
    Runtime_Stack(){
        fp = 0,sp = 0;
        base_memory = nullptr;
    }
    void push(Content c){
        char* ref = base_memory + AllocSize - 1 - fp - sp - 8 + 1;
        for(int i = 0;i < 8;i=i+1,ref++){
            (*ref) = c.chc[i];
        }
        sp += 8;
    }
    void push(char* chs,size_t size){
        char* ref = base_memory + AllocSize - 1 - fp - sp - size + 1;
        for(int i = 0;i < size;i=i+1,ref++){
            (*ref) = chs[i];
        }
        sp += size;
    }
    Content* pop(){
        Content* ref = (Content*)(base_memory + AllocSize - 1 - fp - sp + 1);
        sp -= 8;
        return ref;
    }
    char* pop(size_t size){
        char* ret = base_memory + AllocSize - 1 - fp - sp + 1;
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
        fp = pop()->intc;
        std::cout << "result:" << fp << " , " << sp << std::endl;
    }
};

// 暂时不需要实现这玩意
class Runtime_Heap{
    public:
    std::vector<heap_item_t> list;
    char* base_memory;
    long AllocSize;
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
    Runtime_Heap(std::string& base ,size_t start){
        this->base_memory = (char*)base.data();
        this->start = start;
        this->top = start;
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
        char* ref = (char*)base_memory + list[ret][2];
        for(int i = 0;i < 8;i++,ref++){
            (*ref) = s.chc[i];
        }
        return ret;
    }
    size_t alloc(size_t s){
        size_t ret;
        if((ret = FindFreeBlock(s)) != LONG_MAX);
        else{
            heap_item_t h;
            h[0] = true;
            h[1] = s;
            h[2] = top;
            top += s;
            list.push_back(h);
            ret = list.size() - 1;
        }
        return ret;
    }
    size_t InsertToHeap(std::string& s){
        size_t ret = alloc(s.size());
        char* ref = (char*)base_memory + list[ret][2];
        for(int i = 0;i < s.size();i++,ref++){
            (*ref) = s[i];
        }
        return ret;
    }
    void unlink(size_t id){
        list[id][0] = false;
    }
    void free(size_t addr){
        for(int i = 0;i < list.size();i++){
            if(list[i][2] == addr) list[i][0] = false;
        }
    }
    heap_item_t getInfo(size_t item){
        return list[item]; 
    }
};

struct TSS{
    Content fp,sp;
    Content pc;
    Content regs[32];
    Content basememory;
    Content custom_code_labels;
    Content vheap_start;
    Content _AllocSize;
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
    "equ","neq","maxeq","mineq","max","min",
    "jmp","jt","jf","call",
    "exit","ret","in","out","req","push1b","restore","fork",
    "tclear","tset","trestore", // 分别为清除中断处理状态，设置中断处理状态，返回主任务
    "labelg","labels", // label-get label-set
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

class InterrputControler{
public:
    long HasInterrputSignal;
    bool IsProcessingSignal;
    std::vector<Content> RegisteredProcessingFunction;
};

struct Device{
    char device_name[32];
    void* io_request;
    void* normal_request;
};
struct DevicePackage{
    std::vector<int> require_int_num;
    char device_name[32];
};

#define __XVMDK_HOST
#include "../lib/XVMDK/framework.cpp"

struct device_host{
    std::vector<Device> devlist;
    std::vector<void*>  devhandle;
    
    int LoadVMDevice(void* env,std::string path){
        devhandle.push_back(dlopen(path.c_str(),RTLD_LAZY));
        if(devhandle[devhandle.size() - 1] == nullptr) throw VMError("Load Device Failed:" + path);
        Device dev;
        dev.io_request = dlsym(devhandle[devhandle.size() - 1],"io_request"),sizeof(void*);
        dev.normal_request = dlsym(devhandle[devhandle.size() - 1],"normal_request"),sizeof(void*);
        char* (*f_device_load)(void*) = (char* (*)(void*))dlsym(devhandle[devhandle.size() - 1],"on_device_load");
        char* result = f_device_load(env);
        memcpy(dev.device_name,result,32);
        devlist.push_back(dev);
        return devhandle.size() - 1;
    }

    void device_in(long target,void* env,char* dest){
        void (*io_func)(void* env,char motd,char* dest) = (void (*)(void*,char,char*)) devlist[target].io_request;
        io_func(env,0/*in*/,dest);
    }
    void device_out(long target,void* env,char* dest){
        void (*io_func)(void* env,char motd,char* dest) = (void (*)(void*,char,char*)) devlist[target].io_request;
        io_func(env,1/*out*/,dest);
    }
    void device_request(long target,void* env,char reqid,char* dest){
        void (*normalreq)(void* env,char reqid,char* dest) = (void (*)(void*,char,char*)) devlist[target].normal_request;
        normalreq(env,reqid,dest);
    }
};


device_host devhost;

Content public_temp_place;
class VMRuntime{
    VMExec vme;
    std::string allocated_memory;
    public:
    Content          OriginCodeLabel;
    TSS              backupTSS;
    TSS              mainTSS;
    TSS*             thisTSS;
    int              vme_fd;
    char*            malloc_place;
    char*            tss_alloc;
    size_t           _Alloc_Size;
    bool             regflag;
    PC_Register      pc;
    std::map<std::string,bool> vm_rules;
    Content          regs[32];
    Runtime_Heap     heap;
    Runtime_Stack    stack_a;
    InterrputControler intc;
    ByteCode*        program;
    char*            constant_pool;

    Content& getRegRefernce(int rid){
        return regs[rid];
    }
    void Bind_VMExec(VMExec vme,int unread_rid){
        this->vme = vme;
        this->vme_fd = unread_rid;
    }
    void initTss(){
        mainTSS.fp.intc = stack_a.fp;
        mainTSS.sp.intc = stack_a.sp;
        mainTSS.basememory.intc = 0;
        mainTSS.custom_code_labels.intc = LONG_MAX;
        tss_alloc = malloc_place + mainTSS.basememory.intc;
        mainTSS.vheap_start.intc = (long long)((long long)heap.base_memory - (long long)tss_alloc);
        mainTSS.pc.intc = (long long)((long long)pc.offset - (long long)tss_alloc);
        for(int i = 0;i < 32;i++) mainTSS.regs[i] = regs[i];
        mainTSS._AllocSize.intc = _Alloc_Size;
        thisTSS = &mainTSS;
    }
    void LoadTSS(){
        stack_a.fp = thisTSS->fp.intc;
        stack_a.sp = thisTSS->sp.intc;
        stack_a.base_memory = malloc_place + thisTSS->basememory.intc;
        stack_a.AllocSize = thisTSS->_AllocSize.intc;
        heap.base_memory = malloc_place + thisTSS->basememory.intc;
        heap.start = mainTSS.vheap_start.intc;
        for(int i = 0;i < 32;i++) regs[i] = thisTSS->regs[i];
        pc.offset = (ByteCode*)tss_alloc + thisTSS->pc.intc;
        if(thisTSS->custom_code_labels.intc != LONG_MAX) vme.label_array = (CodeLabel*)(tss_alloc + thisTSS->custom_code_labels.intc);
    }
    void UnLoadTSS(){
        thisTSS->fp.intc = stack_a.fp;
        thisTSS->sp.intc = stack_a.sp;
        thisTSS->basememory.intc = (long long)((long long)stack_a.base_memory - (long long)tss_alloc);
        thisTSS->vheap_start.intc = heap.start;
        if(thisTSS->custom_code_labels.intc != LONG_MAX) thisTSS->custom_code_labels.intc = (long long)thisTSS->custom_code_labels.intc - (long long)malloc_place;
        for(int i = 0;i < 32;i++) thisTSS->regs[i] = regs[i];
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
        else if(a.opid == Address) return tss_alloc + a.c.intc;
        else if(a.opid == Address_Register) return tss_alloc + getRegRefernce(a.c.intc).intc;
        else return nullptr;
    }
    void StartVMProc(){
        for(CodeLabel* i = vme.label_array;i < vme.label_array+vme.head.code_label_count;i++){
            if(std::string(i->label_n) == "_vmstart") pc.offset = program + i->start;
            //if(strcmp(i->label_n,"_main_int")) pc.offset = program + i->start;
        }
        initTss();
        while(pc.offset->c.intc != realmap["exit"]){
            if(intc.HasInterrputSignal != -1){
                backupTSS = mainTSS;
                stack_a.save();
                intc.IsProcessingSignal = true;
                pc.offset = program + vme.label_array[intc.RegisteredProcessingFunction[intc.HasInterrputSignal].intc].start;
                continue;
            }
            //if(pc.offset->c.intc != realmap["ret"]) disasm();
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
            }else if(pc.offset->c.intc == realmap["push1b"]){
                Content _src;
                if(GetMemberAddress(*(pc.offset + 2)) != nullptr) _src = *(Content*)GetMemberAddress(*(pc.offset+2));
                else _src = (pc.offset+2)->c;
                for(int i = 1;i < 8;i++) _src.chc[i] = 0;
                *((Content*)GetMemberAddress(*(pc.offset+1))) = _src;
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
                    regs[i] = *stack_a.pop();
                }
                public_temp_place = *stack_a.pop();
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
                     pc.offset->c.intc == realmap["max"] || pc.offset->c.intc == realmap["min"]   || pc.offset->c.intc == realmap["neq"]
                    ){
                Content _dest,_src;
                Content* tmp = (Content*)GetMemberAddress(*(pc.offset+1));
                if(tmp != nullptr) _dest = *tmp;
                else _dest.intc = (pc.offset+1)->c.intc;
                tmp = (Content*)GetMemberAddress(*(pc.offset+2));
                if(tmp != nullptr) _src = *tmp;
                else _src.intc = (pc.offset+2)->c.intc;
                if(pc.offset->c.intc == realmap["equ"])   regflag = (_dest.intc == _src.intc);
                if(pc.offset->c.intc == realmap["neq"])   regflag = (_dest.intc != _src.intc);
                if(pc.offset->c.intc == realmap["maxeq"]) regflag = (_dest.intc >= _src.intc);
                if(pc.offset->c.intc == realmap["mineq"]) regflag = (_dest.intc <= _src.intc);
                if(pc.offset->c.intc == realmap["max"])   regflag = (_dest.intc > _src.intc);
                if(pc.offset->c.intc == realmap["min"])   regflag = (_dest.intc < _src.intc);
            }else if(pc.offset->c.intc == realmap["jmp"] || pc.offset->c.intc == realmap["jt"] || pc.offset->c.intc == realmap["jf"]){
                Content s;
                if(GetMemberAddress(*(pc.offset + 1)) != nullptr) s = *(Content*)GetMemberAddress(*(pc.offset+1));
                else s = (pc.offset+1)->c;
                if(pc.offset->c.intc == realmap["jmp"]) {pc += s.intc;continue;}
                else if(pc.offset->c.intc == realmap["jt"] && regflag == 1) {pc += s.intc;continue;}
                else if(pc.offset->c.intc == realmap["jf"] && regflag == 0) {pc += s.intc;continue;}
            }else if(pc.offset->c.intc == realmap["call"]){
                //long i=;
                pc = program + vme.label_array[(pc.offset+1)->c.intc].start;
                continue;
            }else if(pc.offset->c.intc == realmap["ret"]){
                char* _Src = GetMemberAddress(*(pc.offset+1));
                stack_a.pop_frame();
                regflag = stack_a.pop(1);
                for(int i = 31;i >= 0;i--){
                    regs[i] = *stack_a.pop();
                }
                public_temp_place = *stack_a.pop();
                long size = (pc.offset+2)->c.intc;
                stack_a.push(_Src,size);
                pc.offset = (ByteCode*)public_temp_place.ptrc;
                while(pc.offset->c.intc != realmap["call"]) pc++;
                pc++;
                continue;
                //pc++;
            }else if(pc.offset->c.intc == realmap["in"] || pc.offset->c.intc == realmap["out"]){
                Content devid;
                if(GetMemberAddress(*(pc.offset + 1)) != nullptr) devid = *(Content*)GetMemberAddress(*(pc.offset+1));
                else devid = (pc.offset+1)->c;
                char* dest = GetMemberAddress(*(pc.offset + 2));
                if(pc.offset->c.intc == realmap["in"]) devhost.device_in(devid.intc,this,dest);
                else devhost.device_out(devid.intc,this,dest);
                //std::cout << dest[0];
            }else if(pc.offset->c.intc == realmap["req"]){
                Content devid,reqid;
                if(GetMemberAddress(*(pc.offset + 1)) != nullptr) devid = *(Content*)GetMemberAddress(*(pc.offset+1));
                else devid = (pc.offset+1)->c;
                if(GetMemberAddress(*(pc.offset + 2)) != nullptr) reqid = *(Content*)GetMemberAddress(*(pc.offset+2));
                else reqid = (pc.offset+2)->c;
                char* dest = GetMemberAddress(*(pc.offset + 3));
                devhost.device_request(devid.intc,this,reqid.intc,dest);
            }else if(pc.offset->c.intc == realmap["fork"]){
                // fork从当前环境复制一个TSS到指定内存地址
                TSS* tss = (TSS*)GetMemberAddress(*(pc.offset+1));
                *tss = *thisTSS;
            }else if(pc.offset->c.intc == realmap["restore"]){
                // restore从指定内存地址恢复执行环境
                TSS* tss = (TSS*)GetMemberAddress(*(pc.offset+1));
                UnLoadTSS(); // 卸载之前的TSS
                thisTSS = tss; // 将当前TSS设置为指定TSS
                continue; // 多任务需要
            }else if(pc.offset->c.intc == realmap["tset"]){
                intc.IsProcessingSignal = true;
            }else if(pc.offset->c.intc == realmap["tclear"]){
                intc.IsProcessingSignal = false;
            }else if(pc.offset->c.intc == realmap["trestore"]){
                mainTSS = backupTSS;
                continue;
            }else if(pc.offset->c.intc == realmap["labelg"]){
                Content* dest = (Content*)GetMemberAddress(*(pc.offset+2));
                Content num = ((Content*)GetMemberAddress(*(pc.offset+2)) == nullptr) ? (pc.offset+2)->c : *(Content*)GetMemberAddress(*(pc.offset+2));
                dest->intc = (long long)(program + vme.label_array[num.intc].start) - (long long)tss_alloc;
            }else if(pc.offset->c.intc == realmap["labels"]){
                Content lblid;
                if(GetMemberAddress(*(pc.offset + 1)) != nullptr) lblid = *(Content*)GetMemberAddress(*(pc.offset+1));
                else lblid = (pc.offset+1)->c;
                Content addr;
                if(GetMemberAddress(*(pc.offset + 2)) != nullptr) addr = *(Content*)GetMemberAddress(*(pc.offset+2));
                else addr = (pc.offset+2)->c;
                vme.label_array[((Content*)GetMemberAddress(*(pc.offset+1)))->intc].start = addr.intc;
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
        heap = Runtime_Heap(allocated_memory,vme.cpool.size+vme.head.code_length * 16+1);
        if(vm_rules["verbose"] == true){
            printf("Xtime VM Core[1.0.01]\nStarting...\n");
        }
        for(int i = 0;i < 33;i++){
            //std::cout << "COMMAND:" << COMMAND_MAP[i] << std::endl;
            realmap[COMMAND_MAP[i]] = i;
        }

        //init intctl
        intc.HasInterrputSignal = -1;
        intc.IsProcessingSignal = false;
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
    Content *c = (Content*)(&rt.tss_alloc[v]);
    out << "Memory Watcher=>" << v << "\n int val=>" << c->intc << "\n Charter Val=>" << std::string(c->chc,8) << "\n";
}
