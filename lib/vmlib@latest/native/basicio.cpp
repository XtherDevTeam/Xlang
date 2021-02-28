#define __XVMDK_CLIENT
#include "../../XVMDK/dlfcn.h"
#include "../../XVMDK/framework.cpp"
#ifdef __WIN32
#include <conio.h>
#endif
#define export_dll extern "C"

VMRuntime* target;

export_dll char* on_device_load(VMRuntime* env){
    target = env;
    std::string s = "BASIC INPUT/OUTPUT DEVICE";
    s.resize(32);
    return (char*)s.c_str();
}

export_dll void io_request(VMRuntime* env,char motd,char* dest){
    if(motd == 0)
    {
        #if (defined _WIN32)
        dest[0] = getch();
        #else
        system("stty raw");
        dest[0]=getchar();
        if(dest[0] == 13) dest[0] = '\n';
        //if(dest[0] == 3) exit(0);
        system("stty cooked");
        #endif
    }else{
        std::cout << dest[0];
    }
}

export_dll void normal_request(void* env,char reqid,char* dest){
    perror("Unsupport this action");
}