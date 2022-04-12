//
// Created by Jerry Chou on 2022/3/22.
//

#include "StringUtils.hpp"

std::string wstring2string(const std::wstring &ws) {
    std::string curLocale = setlocale(LC_ALL, nullptr);

    setlocale(LC_ALL, "chs");

    const wchar_t *Source = ws.c_str();
    size_t d_size = 2 * ws.size() + 1;
    char *Dest = new char[d_size];
    memset(Dest, 0, d_size);
    wcstombs(Dest, Source, d_size);
    std::string result = Dest;
    delete[] Dest;

    setlocale(LC_ALL, curLocale.c_str());

    return result;
}

std::wstring string2wstring(const std::string &s) {
    const char *Source = s.c_str();
    size_t d_size = s.size() + 1;
    auto *Dest = new wchar_t[d_size];
    wmemset(Dest, 0, d_size);
    mbstowcs(Dest, Source, d_size);
    std::wstring result = Dest;
    delete[] Dest;

    return result;
}

bool IsDigit(wchar_t Char) {
    return (Char >= L'0' && Char <= L'9');
}

bool IsAlpha(wchar_t Char) {
    return ((Char >= L'A' && Char <= L'Z')  || (Char >= L'a' && Char <= 'z') || Char == L'_');
}
