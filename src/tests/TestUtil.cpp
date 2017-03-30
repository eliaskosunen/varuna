// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/StringUtils.h"
#include <doctest.h>
#include <string>
#include <vector>

template <typename T>
void testIsChar(T&& f, std::vector<char>&& is, std::vector<char>&& isnot)
{
    auto isfunc = [&f](char c) {
        if(!f(c))
        {
            // Returned false:
            // Failure
            CHECK(c == 0);
        }
    };
    auto isnotfunc = [&f](char c) {
        if(f(c))
        {
            // Returned true:
            // Failure
            CHECK(c == 0);
        }
    };

    for(const auto& c : is)
    {
        isfunc(c);
    }
    for(const auto& c : isnot)
    {
        isnotfunc(c);
    }
}

TEST_CASE("stringutils")
{
    using namespace util::stringutils;

    CHECK(replaceAllCopy("foo", "o", "a") == "faa");
    CHECK(trim("  foo  ") == "foo");
    CHECK(trimConsecutiveSpacesCopy("foo   bar") == "foo bar");
    CHECK(cstrToString("foo") == "foo");
    CHECK(charToString('a') == "a");

    SUBCASE("isCharSomething")
    {
        testIsChar(isCharAlpha,
                   {'a', 'b', 'c', 'd', 'e', 'f', 'q', 'w', 'r', 't', 'y', 'z',
                    'A', 'Y', 'Y', 'L', 'M', 'A', 'O'},
                   {'0', '1', '2', '8', '9', '-', '+', '_', 0x0, ' '});
        testIsChar(isCharDigit, {'0', '1', '2', '8', '9'},
                   {'a', 'z', 'A', 'Z', '[', '-', '%'});
        testIsChar(isCharAlnum,
                   {'a', 'z', 'A', 'Z', '0', '9', 'y', 'E', 's', '4'},
                   {'-', '_', '\'', ']'});
        testIsChar(isCharHexDigit, {'0', '1', '5', '9', 'a', 'f', 'A', 'F'},
                   {'G', 'g', '-', '&'});
        testIsChar(isCharOctDigit, {'0', '1', '7'}, {'8', '9', 'a', 'Z', 'f'});
        testIsChar(isCharBinDigit, {'0', '1'}, {'2', '9', 'A', 'z', '.'});
        testIsChar(isCharWhitespace, {' ', '\t', '\n', '\r'},
                   {'A', 'Z', 'a', 'z', '0', '9', '#', '$'});
    }

    SUBCASE("createEmptyStringWithLength")
    {
        const auto s = createEmptyStringWithLength(10);
        CHECK(s.length() == 10);
        CHECK(s.at(0) == ' ');
        CHECK(s.at(3) == ' ');
        CHECK(s.at(8) == ' ');
    }
}
