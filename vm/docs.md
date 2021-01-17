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
-  Constant Memory (1/4) *for string constant*
-  Registers (32) 
-  - reg30 **memory offset register**<br> 
     reg31 **function args addres register**<br>
     reg32 **return value register**<br>