# Get Started
## DOWNLOAD AND COMPILE IT
first,download this repository to your computer.and chdir to it.
if you don't have g++ compiler?please setting up beforce you compile it
if you finish this step.then execute it:
`g++ ./xlang.cpp -o xlang -g`
`g++ ./vm/vm.cpp -o ./vm/vm -g`
you also can don't append `-g` this arg , that is for debug
## Basic Syntax
### Function declaration
```go
func [return type] [function name](arg0_type:arg0_name,...){
  code here
};

sample here:
func int main(int:argc,ptr_char:argv){
  return 0;
};
```
### Variable declaration
```go
[typename] varname<=[any]>,...;

sample here:
int i=0;
```
### Description:
just like:
```c
// something
```
multi line:
```c
/*
something here
*/
```

### Expression And Statement:
**ATTENTION TO**
- There are no brackets in the expression to modify the priority. The priority is sorted according to four operations. The logical symbol is calculated after the operation, and the addition, subtraction, multiplication and division have priority. In addition, the member expression (.) and the type expression (:) are used for structure and function declaration respectively

- All the expression and statement must has a **SEMICOLON** after it.

### If Statement And Flow control statement

Syntax of If Statement:
```c++
if(expr){
  somthing here
}elif{
  something here
}else{
  same
};
```

Syntax Of flow control statement:
```c++
for(expr0,expr1,expr2){};
while(expr){};
```

### How to import a file
```go
import(headers:"hahaha.xlang",...);
```

### First Program
**Basic Compiler doesn't have crt we'll fix it later(or never fix it : -)**<br>
open an new file and type something in this file:
```go
func int main(){
  return 140734682135840;
};
int printing = main();
```
If no bugs here,it will print