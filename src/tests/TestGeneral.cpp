/*
Copyright (C) 2016-2017 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
