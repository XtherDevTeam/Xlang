# The bytecodes in Xlang2

## Beginning

Xlang2 is a compiled language.
The outputs of source files are `Xclass` structure.
It will execute by XtimeVM (*A virtual machine for Xlang2*)

## Values operating

There is an operand stack in XtimeVM.
It saves local variables and operands.

Of course, you can operate the elements in the stack with some bytecode commands.

### Creating a local variable in stack

Using commands: `push_integer`, `push_decimal`, `push_boolean`

For the following Xlang2 code:
```javascript
var i = 114514;
```
Compiler will create a symbol item for `i`, and generate a command:
```asm
push_integer 114514;
```

It pushed an integer to operand stack, but in fact, it's a local variable index of `i`.

It is the same to initialize a decimal variable or a boolean variable in stack.

## Operating variables in stack
Using commands: 
`duplicate`, `add_integer`, `sub_integer`, `mul_integer`, 
`div_integer`, `mod_integer`,
`add_decimal`, `sub_decimal`, `mul_decimal`,
`div_decimal`, `mod_decimal`, 
`binary_or`, `binary_and`, `binary_xor`,
`binary_lm`, `binary_rm`,
`logic_mt`, `logic_lt`, `logic_me`, `logic_le`,
`logic_and`, `logic_or`,
`increment_integer`, `decrement_integer`,
`increment_decimal`, `decrement_decimal`,
`store_integer`, `store_decimal`, `store_boolean`,

For following Xlang2 code:
```javascript
var i = 114514;
var j = 1919810;
i = i + 1;
j = i + j;
```

Compiler will generate these following codes:
```asm
push_integer 114514; # push an integer 114514 to index [1]
push_integer 1919810; # push an integer 1919810 to index [2]
duplicate 1; # duplicate value from index [1] to index [3]
push_integer 1; # push an integer 1 to index [4]
add_integer; # add index [3] and index [4] and push result to index [3]
store_integer 1; # storage index [3] into index [1] and pop index [3]
duplicate 1; # duplicate value from index [1] to index [3]
duplicate 2; # duplicate value from index [2] to index [4]
add_integer; # add index [3] and index [4] and push result to index [3]
storage_integer 2; # storage index [3] into index [2] and pop index [3]
```

**Binary operator commands** will pop out 2 values from stack, calculate them and put the result into stack.

**Unary operator commands** will pop out 1 value from stack, calculate it  and put the result into stack.

**Storage commands** is for assignments. It pops a value from stack, and save it to designated local variable index.

