// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/StreamReader.h"
#include "util/StringUtils.h"
#include <doctest.h>
#include <iostream>
#include <string>

TEST_CASE("StreamReader tests")
{
    util::StreamReader sr;

    SUBCASE("Reading file")
    {
        std::string str = sr.readFile("./test.txt");

        REQUIRE(str == "TEST \n");
    }
}

TEST_CASE("StringUtils tests")
{
    using namespace util;

    REQUIRE(StringUtils::replaceAllCopy("foo", "o", "a") == "faa");
    REQUIRE(StringUtils::ltrimCopy("   foo") == "foo");
    REQUIRE(StringUtils::rtrimCopy("foo   ") == "foo");
    REQUIRE(StringUtils::trimCopy("  foo  ") == "foo");
    REQUIRE(StringUtils::trimConsecutiveSpacesCopy("foo   bar") == "foo bar");
    REQUIRE(StringUtils::cstrToString("foo") == "foo");
    REQUIRE(StringUtils::charToString('a') == "a");
    REQUIRE(StringUtils::isCharWhitespace(' ') == true);
    REQUIRE(StringUtils::isCharWhitespace('a') == false);
}
