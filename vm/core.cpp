#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include "../compiler/basic_type.cpp"
#include "const.cpp"

using namespace std;
typedef unsigned long int addr_t;

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

struct VMExec{
    VMExecHeader head;
    CodeLabel* label_array;
    ByteCode* code_array;
};

void VMExec_Serialization(char* filename,VMExec vme){
    int fd = open(filename,O_RDWR|O_CREAT);
    if(fd == -1) perror("File open error!");
    write(fd,&vme.head,sizeof(vme.head));
    write(fd,vme.label_array,vme.head.code_label_count * 48);
    write(fd,vme.code_array,vme.head.code_length * 9);
}

int LoadVMExec(char* filename,VMExec* vme){
    int fd = open(filename,O_RDWR|O_CREAT);
    if(read(fd,&vme->head,sizeof(VMExecHeader)) == -1)  return VM_FAILED_OPEN;
    if(vme->head.hash != 0x114514ff) return VM_INVALID_HASH;
    vme->label_array = (CodeLabel*)malloc(vme->head.code_label_count * 48);
    if(vme->label_array == NULL) return VM_FAILED_OPEN;
    else if(read(fd,vme->label_array,vme->head.code_label_count * 48) == -1) return VM_FAILED_OPEN;
    vme->code_array = (ByteCode*)malloc(vme->head.code_length * 9);
    if(vme->code_array == NULL) return VM_FAILED_OPEN;
    else if(read(fd,vme->code_array,vme->head.code_length * 9) == -1) return VM_FAILED_OPEN;
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

class VMRuntime{
    VMExec vme;
    char* malloc_place;
    public:
    Content      regs[32];
    Runtime_Heap heap;
    ByteCode*    program;

    Content& getRegRefernce(int rid){
        return regs[rid];
    }
    int Bind_VMExec(VMExec vme){
        this->vme = vme;
    }
    void Run(addr_t _AllocSize){
        malloc_place = (char*)malloc(_AllocSize);
        program = (ByteCode*)malloc_place;
        memcpy(program,vme.code_array,vme.head.code_length * 9);
    }
    VMRuntime(VMExec vme){
        memset(&regs,0,32*sizeof(Content));
        Bind_VMExec(vme);
    }
};