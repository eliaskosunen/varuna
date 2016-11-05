/*
Copyright (C) 2016 Elias Kosunen

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

#include <string>
#include <iostream>

#include "catch.hpp"

#include "util/StreamReader.h"
#include "util/StringUtils.h"

TEST_CASE("StreamReader tests", "[util]")
{
	util::StreamReader sr;

	SECTION("Reading file")
	{
		std::string str = sr.readFile("./test.txt");

		REQUIRE(str == "TEST \n");
	}
}

TEST_CASE("StringUtils tests", "[util]")
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
