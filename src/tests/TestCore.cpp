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

#include "catch.hpp"

#include <string>

#include "core/lexer/Token.h"

TEST_CASE("Test Token", "[core]")
{
	using namespace core::lexer;

	Token t;
	REQUIRE(t.getType() == TOKEN_DEFAULT);
	t.setType(TOKEN_UNKNOWN);
	REQUIRE(t.getType() == TOKEN_UNKNOWN);
	REQUIRE(t.typeToString() == "UNKNOWN");

	SECTION("Test params")
	{
		REQUIRE(t.isParamsEmpty() == true);
		REQUIRE(t.getParam("UNEXISTANT") == Token::DEFAULT_PARAM);

		t.setParam("KEY", "VALUE");
		REQUIRE(t.paramExists("KEY") == true);
		REQUIRE(t.getParam("KEY") == "VALUE");
		REQUIRE(t.isParamsEmpty() == false);

		t.setParam("KEY", "NEWVALUE");
		REQUIRE(t.paramExists("KEY") == true);
		REQUIRE(t.getParam("KEY") == "NEWVALUE");
		REQUIRE(t.isParamsEmpty() == false);

		t.setParam("KEY", "NOTOVERRIDE", false);
		REQUIRE(t.paramExists("KEY") == true);
		REQUIRE(t.getParam("KEY") == "NEWVALUE");
		REQUIRE(t.isParamsEmpty() == false);

		t.setParam("NEWKEY", "VALUE");
		REQUIRE(t.paramExists("NEWKEY") == true);
		REQUIRE(t.paramExists("KEY") == true);
		REQUIRE(t.getParam("NEWKEY") == "VALUE");
		REQUIRE(t.getParam("KEY") == "NEWVALUE");
		t.removeParam("NEWKEY");
		REQUIRE(t.paramExists("NEWKEY") == false);
		REQUIRE(t.paramExists("KEY") == true);
		REQUIRE(t.getParam("NEWKEY") == Token::DEFAULT_PARAM);

		t.clearParams();
		REQUIRE(t.isParamsEmpty() == true);
	}
}
