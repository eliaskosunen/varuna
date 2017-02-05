// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/StreamReader.h"
#include "util/StringUtils.h"
#include <doctest.h>
#include <string>
#include <vector>

TEST_CASE("StreamReader tests")
{
    util::StreamReader sr;

    SUBCASE("Reading file")
    {
        std::string str = sr.readFile("./test.txt");

        CHECK(str == "TEST \n");
    }
}

TEST_CASE("StringUtils tests")
{
    using namespace util;

    CHECK(StringUtils::replaceAllCopy("foo", "o", "a") == "faa");
    CHECK(StringUtils::ltrimCopy("   foo") == "foo");
    CHECK(StringUtils::rtrimCopy("foo   ") == "foo");
    CHECK(StringUtils::trimCopy("  foo  ") == "foo");
    CHECK(StringUtils::trimConsecutiveSpacesCopy("foo   bar") == "foo bar");
    CHECK(StringUtils::cstrToString("foo") == "foo");
    CHECK(StringUtils::charToString('a') == "a");
}

template <typename T>
void testIsChar(T&& f, std::vector<util::char_t>&& is,
                std::vector<util::char_t>&& isnot)
{
    auto isfunc = [&f](util::char_t c) {
        if(!f(c))
        {
            // Returned false:
            // Failure
            CHECK(c == 0);
        }
    };
    auto isnotfunc = [&f](util::char_t c) {
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

TEST_CASE("StringUtils isCharSomething")
{
    using namespace util::StringUtils;

    testIsChar(isCharAlpha, {'a', 'b', 'c', 'd', 'e', 'f', 'q', 'w', 'r', 't',
                             'y', 'z', 'A', 'Y', 'Y', 'L', 'M', 'A', 'O'},
               {'0', '1', '2', '8', '9', '-', '+', '_', 0x0, ' '});
    testIsChar(isCharDigit, {'0', '1', '2', '8', '9'},
               {'a', 'z', 'A', 'Z', '[', '-', '%'});
    testIsChar(isCharAlnum, {'a', 'z', 'A', 'Z', '0', '9', 'y', 'E', 's', '4'},
               {'-', '_', '\'', ']'});
    testIsChar(isCharHexDigit, {'0', '1', '5', '9', 'a', 'f', 'A', 'F'},
               {'G', 'g', '-', '&'});
    testIsChar(isCharOctDigit, {'0', '1', '7'}, {'8', '9', 'a', 'Z', 'f'});
    testIsChar(isCharBinDigit, {'0', '1'}, {'2', '9', 'A', 'z', '.'});
    testIsChar(isCharWhitespace, {' ', '\t', '\n', '\r'},
               {'A', 'Z', 'a', 'z', '0', '9', '#', '$'});
}
