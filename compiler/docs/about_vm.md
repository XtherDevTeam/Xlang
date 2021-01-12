# Function Definition Statement
```java
func int main(int:a,int:b){
    return 1;
}
```
reg30: args addres register <br>
以上代码应该将被翻译成:
```asm
main:
mov_m [0],[reg30],8;
add reg30,8;
mov_m [0],[reg30],8;
mov 1,reg10;
```
```asm
mov [0],0;
mov [8],0;
mov reg30,0;
call main;
```