// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include <doctest.h>

namespace tests
{
inline unsigned int factorial(unsigned int number)
{
    return number > 1 ? factorial(number - 1) * number : 1;
}
}

TEST_CASE("General test")
{
    REQUIRE(tests::factorial(0) == 1);
    REQUIRE(tests::factorial(1) == 1);
    REQUIRE(tests::factorial(2) == 2);
    REQUIRE(tests::factorial(3) == 6);
    REQUIRE(tests::factorial(10) == 3628800);
}
