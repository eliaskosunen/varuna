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

#include "catch/catch.hpp"

#include <string>

#include "core/lexer/Token.h"

TEST_CASE("Test Token", "[core]")
{
	using namespace core::lexer;

	Token t;
	REQUIRE(t.getType() == TOKEN_DEFAULT);
	REQUIRE(t.getCategory() == TOKEN_CAT_DEFAULT);
	t.setType(TOKEN_UNKNOWN);
	REQUIRE(t.getType() == TOKEN_UNKNOWN);
	REQUIRE(t.typeToString() == "UNKNOWN");
	REQUIRE(t.getCategory() == TOKEN_CAT_UNKNOWN);
	REQUIRE(t.categoryToString() == "UNKNOWN");
}
