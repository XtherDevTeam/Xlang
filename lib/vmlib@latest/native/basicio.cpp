#define __XVMDK_CLIENT
#include "../../XVMDK/dlfcn.h"
#include "../../XVMDK/framework.cpp"
#define export_dll extern "C"

VMRuntime* target;

export_dll char* on_device_load(VMRuntime* env){
    target = env;
    std::string s = "BASIC INPUT/OUTPUT DEVICE";
    s.resize(32);
    return (char*)s.c_str();
}

export_dll void io_request(VMRuntime* env,char motd,char* dest){
    if(motd = 0){
        dest[0] = read(0,dest,1);
    }else{
        write(1,dest,1);
    }
}

export_dll void normal_request(){
    perror("Unsupport this action");
}