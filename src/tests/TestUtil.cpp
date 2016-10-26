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

#include "util/CommandLineParser.h"
#include "util/StreamReader.h"
#include "util/StringUtils.h"

TEST_CASE("CommandLineParser tests", "[util]")
{
	int argc1 = 3;
	const char *argv1[] = {"foo", "arg1", "arg2"};
	util::CommandLineParser clp1(argc1, argv1);

	REQUIRE(clp1.size() == 2);
	REQUIRE(clp1.empty() == false);
	REQUIRE(clp1.optionExists("arg1") == true);
	REQUIRE(clp1.optionExists("arg2") == true);
	REQUIRE(clp1.optionExists("arg3") == false);

	int argc2 = 4;
	const char *argv2[] = {"foo", "-f", "filename", "-h"};
	util::CommandLineParser clp2(argc2, argv2);
	REQUIRE(clp2.size() == 3);
	REQUIRE(clp2.empty() == false);
	REQUIRE(clp2.optionExists("-f") == true);
	REQUIRE(clp2.optionExists("-h") == true);
	REQUIRE(clp2.getOption("-f") == "filename");

	int argc3 = 1;
	const char *argv3[] = {"foo"};
	util::CommandLineParser clp3(argc3, argv3);
	REQUIRE(clp3.size() == 0);
	REQUIRE(clp3.empty() == true);
}

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
