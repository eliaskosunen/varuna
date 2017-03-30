// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <cstdint>
#include <limits>

namespace util
{
template <typename T>
inline std::size_t numDigits(T x)
{
    std::size_t digits = 0;
    if(x <= 0)
    {
        digits = 1;
    }
    while(x)
    {
        x /= 10;
        digits++;
    }
    return digits;
}

template <>
inline std::size_t numDigits(int32_t x)
{
    if(x == std::numeric_limits<int32_t>::min())
    {
        return 11;
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
} // namespace util
