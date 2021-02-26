### 语言 [README.md](README.md) | [README_zh-cn.md](README_zh-cn.md)
### 更新日志 [updates.log](updates.log)
# 开始
## 下载并编译
- 首先，将此存储库下载到您的计算机。
- 如果您没有g ++编译器？请进行设置以确保您对其进行编译
- 如果完成此步骤，请执行以下步骤：
`g ++ ./xlang.cpp -o xlang -g`
`g ++ ./vm/vm.cpp -o ./vm/vm -g`
你不能附加`-g`这个命令行参数，这是为了调试

- 如果您使用的是macOS，则缺少bits/stdc ++。h。您可以安装brew并使用g ++-10对其进行编译。
## 基本语法
### 函数声明
```go
func [return type] [function name](arg0_type:arg0_name,...){
  code here
};

sample here:
func int main(int:argc,ptr_char:argv){
  return 0;
};
```
### 变量声明
```go
[typename] varname<=[any]>,...;

sample here:
int i=0;
```
### 注释：
就像：
```c
// something
```
多行注释：
```c
/*
something here
*/
```

### 表达式和语句：
**注意**
- 表达式中没有括号可以修改优先级。根据四个操作对优先级进行排序。逻辑符号是在运算后计算的，加，减，乘和除具有优先权。此外，成员表达式(.)和类型表达式(:)分别用于结构和函数声明

- 所有表达式和语句后都必须带有** SEMICOLON **。

### If语句和流控制语句

If语句的语法：
```c++
if(expr){
  somthing here
}elif{
  something here
}else{
  same
};
```

流控制语句的语法：
```c++
for(expr0,expr1,expr2){};
while(expr){};
```
### 如何导入文件
```go
import(headers:"hahaha.xlang",...);
```

### 第一个程序
** Basic Compiler没有crt(CRunTime)，我们稍后将对其进行修复(或从不对其进行修复:-)** <br>
打开一个新文件，然后在此文件中输入以下内容：
```go
ptr_char p="Hello,World";
```
如果这里没有错误，它将打印：
```bash
ubuntu@VM-0-16-ubuntu:~/Xlang$ ./vm/vm
Xtime VM Core[1.0.01]
Starting...
mov reg0,0;
mov_m [11],reg0,8;
Memory Watcher=>0
 int val=>8022930118255863112
 Charter Val=>Hello,Wo
ubuntu@VM-0-16-ubuntu:~/Xlang$ 
```
太酷了，该字符串已写入常量池中。 
