# Docs About VMExecutableFile

## VMExecFile Struct

```
___________________________________________________
|Name             | Size of 32bit | Size of 64bit |
|=================|===============|===============|
| VMExec Header   | 85            | 93            |
|=================|===============|===============|
|ExecuteHash      | 4             | 8             |
|SupportVMBuild   | 2             | 2             |
|Code Label Count | 4             | 8             |
|Code length      | 8             | 8             |
|Author Message   | 67            | 67            |
|=================|===============|===============|
| VMExecBody      | unknown       | unknown       |
|=================|===============|===============|
|Label Array      | count*48      | count*48      |
|Code Array       | length*9      | length*9      |
---------------------------------------------------
```

## Q: How to execute a vm?
A:
```c++
VMExec vme;
LoadVMExec("filename here",&vme);
VMRuntime rt;
rt.Bind_VMExec(vme);
rt.Run(<memsize default 4096*1024>);
```

## VM

### Core:
-  Heap Memory (1/4) *for malloc and static memory*
-  Program Memory (1/4)
-  Stack Memory (1/4)
-  Constant Memory (1/4) *for std::string constant*
-  Registers (32) 
-  - reg30 **memory offset register**<br> 
     reg31 **function args addres register**<br>
     reg32 **return value register**<br>

## ConstantPool
- Q: What is Constant Pool?
- A: It's a block of init memory,it has all of static variable 's init value
- Q: How does it work?
```c++
ptr char a="Hello,World"
```
Covert to asm:
```c++
@constpool "Hello,World";// load to constant pool
push 0;
```

```
__________________________
| Item            | Size |
--------------------------
| Count           | 8    |
| Item Array      | c*
--------------------------
```

## About Device Dymlic Library

```c++
// A device source code sample
// By xiaokang00010 at 2021/1/22
#include "vm/core.cpp"
 

DevicePackage init_device(){
     // How to know it's an interput device?
     // That's easy,becaude int device name must be "Basic Interput Device"
     DevicePackage ret;
     strcpy(ret.device_name,"Basic Interput Device");
}
```