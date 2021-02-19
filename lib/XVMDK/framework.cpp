#pragma once

#ifdef __XVMDK_CLIENT
#include "../../vm/core.cpp"
size_t heap_alloc(VMRuntime* env,size_t need){
    size_t s = env->heap.getInfo(env->heap.alloc(need))[2];
    return s;
}
size_t heap_free(VMRuntime* env,size_t addr){
    env->heap.free(addr);
}
char* getRealAddr(VMRuntime* env,size_t addr){
    return env->malloc_place + addr;
}
#endif