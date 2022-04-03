# Array design in Xlang2

Creating array in Xlang 2 is easy, like these following codes:
```java
Integer[][] IntArray = new Integer[114][514];
```

It will generate a `Typename derive` in compiling-time.

`Typename derive` saves message of this array.

In this example, its typename derive like this:
```markdown
OriginalType        Integer
AccessDescriptor    Public
VariableDescriptor  None
DeriveKind          Array
ArrayDimension      2
```

And compiler will generate these following codes:
```asm
push_integer 114
push_integer 514
new_multi_dimension_array_int 2
```

The `new` command will creating an object in XtimeVM Heap according the type of the operand.