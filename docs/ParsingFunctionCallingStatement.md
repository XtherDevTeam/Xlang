# The bytecode generating of FunctionCallingStatement and FunctionDefinition

Xlang2 needn't save anything about function return address. Function saves its own bytecodes in a XlangFunction
structure, compile won't merge them into one bytecode command array. Program counter in XtimeVM is saved with a local
variable in `InvokeFunction(XFunctionIndexType Idx)`. This is why, Xlang2 needn't save the function return address. When
XtimeVM starts invoke a function, it will configure the stack frame, and call `InvokeFunction(XFunctionIndexType Idx)`
method in XtimeVM class, and initialize a program counter in a local variable, then start bytecode interpretation.

```c
public Integer TestFunc(Integer a, Integer b) {
    Integer c = 1;
    return a + b + c;
};
```

For these codes, compiler outputs:

```asm
Test$TestFunc:
    push_integer 1
    duplicate 0
    duplicate 1
    add_integer
    duplicate 2
    add_integer
    return
```

It is also easy to generate the bytecodes of a FunctionCallingStatement:

Now we have a function like these following codes:

```c
public Integer TestFunc(Integer a, Integer b) {
    Integer c = 1;
    return a + b + c;
};
```

and we gonna to call it with following codes:

```c
TestFunc(114, 514);
```

For this FunctionCallingStatement, compiler outputs:

```asm
push_integer 114
push_integer 514
push_function TestFunc
invoke
```

Function arguments store into stack by the order of arguments, from left to right. This is for stack frame configuring,
and function its own bytecode.