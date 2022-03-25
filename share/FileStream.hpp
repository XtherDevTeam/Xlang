//
// Created by Jerry Chou on 2022/3/22.
//

#ifndef XLANG2_FILESTREAM_HPP
#define XLANG2_FILESTREAM_HPP

#include "IOStream.hpp"
#include "StringUtils.hpp"

#include <cstdlib>

class FileStream : public IOStream {
    FILE *fp;
public:
    FileStream();

    FileStream(const XString& Filename, const XString& Mode);

    XString Read(XInteger Size) override;

    void Write(XString Dest) override;

    XBoolean IsDone() override;

    ~FileStream();
};


#endif //XLANG2_FILESTREAM_HPP
