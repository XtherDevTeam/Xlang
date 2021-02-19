#ifndef _DLFCN_INCLUDED
#define _DLFCN_INCLUDED

#if defined(_WIN32)||defined(_WIN64)
#include<windows.h>
#define RTLD_LAZY (0)
#define RTLD_NOW (0)
#define RTLD_LOCAL (0)
#define RTLD_GLOBAL (0)
#define RTLD_WORLD (0)
#define dlopen(l,x) (LoadLibrary(l))
#define dlsym(p,f) ((void*)GetProcAddress((HMODULE)p,f))
#define dlclose(p) (FreeLibrary((HMODULE)p))
char* dlerror(){char* w;FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR)&w,0,NULL);return w;}
#else
#include<dlfcn.h>
#endif
#endif