#define __XVMDK_CLIENT
#include "../../XVMDK/dlfcn.h"
#include "../../XVMDK/framework.cpp"
#define export_dll extern "C"

VMRuntime* target;

export_dll char* on_device_load(VMRuntime* env){
    target = env;
    return "VMlib Command Set             \0";
}

export_dll void io_request(void* env,char motd,char* dest){
    perror("VMlib isn't an io device");
    return;
}

export_dll void normal_request(void* env,char reqid,char* dest){
    if(reqid == 0){
        char* realdest = ((VMRuntime*)env)->malloc_place + ((Content*)dest)->intc;
        std::cout << "ready to allocate: " << ((Content*)realdest)->intc << "\t reg0's content: " << ((Content*)dest)->intc << std::endl;
        long s=heap_alloc((VMRuntime*)env,((Content*)realdest)->intc);
        ((Content*)dest)->intc = s;
        std::cout << "now dest's content: " << ((Content*)dest)->intc << "\t Should in here: " << s << std::endl;
    }else if(reqid == 1){
        ((Content*)dest)->intc = heap_free((VMRuntime*)env,(*(Content*)dest).intc);
    }
    return;
}