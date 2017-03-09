// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <cstdint>
#include <cstdlib>

#define VA_MANGLE(name) __va_##name

using va_i8 = int8_t;
using va_i16 = int16_t;
using va_i32 = int32_t;
using va_i64 = int64_t;
using va_size = std::size_t;

using va_f32 = float;
using va_f64 = double;

using va_bool = bool;
using va_byte = uint8_t;

using va_char = char32_t;
using va_bchar = char;

struct va_string
{
    va_size len;
    va_bchar* ptr;
};
using va_cstring = va_bchar*;

// C compatibility functions
extern "C" {
va_string VA_MANGLE(cstr_to_str)(va_cstring);
va_cstring VA_MANGLE(str_to_cstr)(va_string);
}

// C standard library functions
extern "C" {
va_string VA_MANGLE(stralloc)(va_size);
va_cstring VA_MANGLE(cstralloc)(va_size);

void VA_MANGLE(strfree)(va_string);
void VA_MANGLE(cstrfree)(va_cstring);

// stdio.h
va_char VA_MANGLE(getchar)();
va_bool VA_MANGLE(gets)(va_string, va_i32);
va_bool VA_MANGLE(getsc)(va_cstring, va_i32);

va_char VA_MANGLE(putchar)(va_char);
va_char VA_MANGLE(puts)(va_string);
va_char VA_MANGLE(putsc)(va_cstring);

void VA_MANGLE(perror)(va_string);
void VA_MANGLE(perrorc)(va_cstring);

va_char VA_MANGLE(eof)();
}
