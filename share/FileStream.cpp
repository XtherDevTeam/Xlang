//
// Created by Jerry Chou on 2022/3/22.
//

#include "FileStream.hpp"

FileStream::FileStream() : fp(nullptr) {}

FileStream::FileStream(const XString& Filename, const XString& Mode) {
    fp = fopen(wstring2string(Filename).c_str(), wstring2string(Mode).c_str());
}

XString FileStream::Read(XInteger Size) {
    std::string dest;
    dest.resize(Size);
    fread(dest.data(), Size, 1, fp);
    return string2wstring(dest);
}

void FileStream::Write(XString Dest) {
    std::string dest = wstring2string(Dest);
    fwrite(dest.c_str(), dest.size(), 1, fp);
}

XBoolean FileStream::IsDone() {
    return feof(fp);
}

FileStream::~FileStream() {
    if (fp != nullptr) {
        fclose(fp);
    }
}
