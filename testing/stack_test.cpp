#include "../vm/core.cpp"

Content ContentMake(long inc){
    Content c;
    c.intc = inc;
    return c;
}

int main(){
    std::string base;base.resize(1024);
    Runtime_Stack rs(base);
    rs.push(ContentMake(114514));
    rs.push(ContentMake(233));
    //rs.output(std::cout);
    std::cout << rs.pop().intc << std::endl;
    std::cout << rs.pop().intc << std::endl;
}