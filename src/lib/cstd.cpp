// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "cstd.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

va_cstring VA_MANGLE(str_to_cstr)(va_string str)
{
    auto cstr = VA_MANGLE(cstralloc)(str.len + 1);
    std::memcpy(cstr, str.ptr, str.len);
    cstr[str.len] = '\0';
    return cstr;
}
va_string VA_MANGLE(cstr_to_str)(va_cstring cstr)
{
    const auto len = std::strlen(cstr);
    auto str = VA_MANGLE(stralloc)(len);
    std::strncpy(str.ptr, cstr, len);
    return str;
}

va_string VA_MANGLE(stralloc)(va_size size)
{
    auto ptr = static_cast<va_bchar*>(std::malloc(size * sizeof(va_bchar)));
    if(!ptr)
    {
        std::fputs("Heap allocation failed: ", stderr);
        std::fputs(std::strerror(errno), stderr);
        std::abort();
    }
    return {size, ptr};
}
va_cstring VA_MANGLE(cstralloc)(va_size size)
{
    auto ptr = static_cast<va_bchar*>(std::malloc(size * sizeof(va_bchar)));
    if(!ptr)
    {
        std::fputs("Heap allocation failed: ", stderr);
        std::fputs(std::strerror(errno), stderr);
        std::abort();
    }
    return ptr;
}

void VA_MANGLE(strfree)(va_string str)
{
    free(str.ptr);
}
void VA_MANGLE(cstrfree)(va_cstring str)
{
    free(str);
}

va_char VA_MANGLE(getchar)()
{
    return static_cast<va_char>(std::getchar());
}
va_bool VA_MANGLE(gets)(va_string str, va_i32 maxSize)
{
    auto ret = std::fgets(str.ptr, maxSize, stdin) != nullptr;
    str.len = std::strlen(str.ptr);
    return ret;
}
va_bool VA_MANGLE(getsc)(va_cstring str, va_i32 maxSize)
{
    return std::fgets(str, maxSize, stdin) != nullptr;
}

va_char VA_MANGLE(putchar)(va_char ch)
{
    return static_cast<va_char>(std::putchar(static_cast<int>(ch)));
}
va_char VA_MANGLE(puts)(va_string str)
{
    auto s = VA_MANGLE(str_to_cstr)(str);
    auto ret = VA_MANGLE(putsc)(s);
    VA_MANGLE(cstrfree)(s);
    return ret;
}
va_char VA_MANGLE(putsc)(va_cstring str)
{
    return static_cast<va_char>(std::puts(str));
}

void VA_MANGLE(perror)(va_string str)
{
    auto s = VA_MANGLE(str_to_cstr)(str);
    VA_MANGLE(perrorc)(s);
    VA_MANGLE(cstrfree)(s);
}
void VA_MANGLE(perrorc)(va_cstring str)
{
    std::perror(str);
}

va_char VA_MANGLE(eof)()
{
    return static_cast<va_char>(EOF);
}
