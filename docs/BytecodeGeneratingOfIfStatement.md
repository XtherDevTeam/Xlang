# Bytecode generating of If statement

If statement is a kind of **condition statement**.

For example, for these following codes:
```c++
public Integer A = 114514;
if (A == 114514) {
    A = 1919810;
} else {
    A = 0;
};
```

Compiler generates:

```asm
/* public Integer A = 114514; */
push_integer 0;
push_integer 114514;
store 0;

/* if (a == 114514) */
duplicate 0;
push_integer 114514;
logic_eq;
jump_if_false 5;
/* { ... } */
push_integer 1919810;
store 0;
jump 2;
/* else { ... } */
push_integer 0;
store 0;
```