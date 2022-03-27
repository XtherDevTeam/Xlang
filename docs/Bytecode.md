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
`int_to_deci`, `bool_to_int`, `deci_to_int`, `int_to_bool`, `deci_to_bool`


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

**Storage commands** are for assignments. It pops a value from stack, and save it to designated local variable index.

**Casting commands** are for operating of elements with different types, for following codes:
```javascript
var i = 0;
i += 114514 + true;
```
Compiler will generate these following codes:
```asm
push_integer 0; # push an integer 0 to index [1]
duplicate 1; # duplicate value from index [1] to index [2]
push_integer 114514; # push an integer 1 to index [3]
push_boolean true;  # push an boolean value true to index [4]
bool_to_int; # case boolean value index [4] to integer
add_integer; # add index [3] with index[4] and push the result to index [3]
add_integer; # same type, compiler doesn't need to case type
storage_integer 1; # storage result to index [1]
```
If the type of left side tree of binary operation is different from the right side tree,
compiler will try to cast one of them to make sure they are the same type.

**Integer + Decimal**: Compiler will case decimal value to integer.

**Decimal + Integer**: Compiler will case integer value to decimal.

**In summary**, compiler will follow the type of left tree, and if types of left side and right side is not the same,
compiler will try to cast type of right side tree to type of left side tree.
It's the same to assignment expressions.