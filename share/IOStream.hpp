//
// Created by Jerry Chou on 2022/3/22.
//

#ifndef XLANG2_IOSTREAM_HPP
#define XLANG2_IOSTREAM_HPP

#include "config.hpp"

class IOStream {
public:
    IOStream();
    virtual XString Read(XInteger Size);
    virtual void Write(XString Dest);
    virtual XBoolean IsDone();
};


#endif //XLANG2_IOSTREAM_HPP
