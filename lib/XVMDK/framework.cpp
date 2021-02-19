#pragma once

#ifdef __XVMDK_CLIENT
#include "../../vm/core.cpp"
long heap_alloc(VMRuntime* env,size_t need){
    long s = env->heap.getInfo(env->heap.alloc(need))[2];
    std::cout << "alloc result:" << s << std::endl; 
    return s;
}
long heap_free(VMRuntime* env,size_t addr){
    env->heap.free(addr);
    return 1;
}
char* getRealAddr(VMRuntime* env,size_t addr){
    return env->malloc_place + addr;
}
#endif