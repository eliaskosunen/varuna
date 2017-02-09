// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <cstdint>
#include <limits>

namespace util
{
template <typename T>
inline std::size_t numDigits(T number)
{
    std::size_t digits = 0;
    if(number <= 0)
    {
        digits = 1;
    }
    while(number)
    {
        number /= 10;
        digits++;
    }
    return digits;
}

template <>
inline std::size_t numDigits(int32_t x)
{
    if(x == std::numeric_limits<int32_t>::min())
    {
        return 10 + 1;
    }
    if(x < 0)
    {
        return numDigits(-x) + 1;
    }

    if(x < 10)
    {
        return 1;
    }
    if(x < 100)
    {
        return 2;
    }
    if(x < 1000)
    {
        return 3;
    }
    if(x < 10'000)
    {
        return 4;
    }
    if(x < 100'000)
    {
        return 5;
    }
    if(x < 1'000'000)
    {
        return 6;
    }
    if(x < 10'000'000)
    {
        return 7;
    }
    if(x < 100'000'000)
    {
        return 8;
    }
    if(x < 1000'000'000)
    {
        return 9;
    }
    return 10;
}

template <>
inline std::size_t numDigits(char n)
{
    static char x[256] = {0};
    if(x[0] == 0)
    {
        for(char c = 1; c != 0; c++)
        {
            x[static_cast<std::size_t>(c)] =
                static_cast<char>(numDigits(static_cast<int32_t>(c)));
        }
        x[0] = 1;
    }
    return static_cast<std::size_t>(x[static_cast<std::size_t>(n)]);
}
}
