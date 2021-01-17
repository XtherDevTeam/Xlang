union Content{
    long int intc;
    double dblc;
    char chc[8];
};

class ASM_Arg{
    enum{
        REG,
        ADDR,
        NUM,
    } type;
    Content c;
    ASM_Arg(int t,long int c){
        this->c.intc = c;
        if(t == 0) type = REG;
        if(t == 1) type = ADDR;
        if(t == 2) type = NUM;
    }
    ASM_Arg(double dbl){
        this->c.dblc = dbl;
        type = NUM;
    }
    ASM_Arg(char ch[8]){
        memcpy(this->c.chc,ch,8);
        type = NUM;
    }
};