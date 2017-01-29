// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

namespace util
{
template <typename T>
bool isPowerOf(T num, T exp)
{
    while(num % exp == 0)
    {
        num /= exp;
    }
    return num == 1;
}
}
