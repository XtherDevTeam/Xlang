# Get Started
## DOWNLOAD AND COMPILE IT
first,download this repository to your computer.and chdir to it.
if you don't have g++ compiler?please setting up beforce you compile it
if you finish this step.then execute it:
`g++ ./xlang.cpp -o xlang -g`
`g++ ./vm/vm.cpp -o ./vm/vm -g`
you alse can don't append `-g` this arg , that is for debug
## Basic Syntax
Function declaration
```go
func [return type] [function name](arg0_type:arg0_name,...){
  code here
};

sample here:
func int main(int:argc,ptr_char:argv){
  return 0;
};
```
Variable declaration
```go
[typename] varname<=[any]>,...;

sample here:
int i=0;
```
