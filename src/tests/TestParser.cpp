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

#include "doctest.h"

#include "core/parser/Parser.h"
#include "core/lexer/Lexer.h"
#include "util/Logger.h"

core::lexer::TokenVector runLexer(const std::string &code)
{
	using namespace core::lexer;

	Lexer l(code, TEST_FILE);
	return l.run();
}

core::parser::Parser parse(const std::string &code)
{
	using namespace core::parser;

	Parser p(runLexer(code));
	p.run();
	return p;
}

TEST_CASE("Test parser")
{
	using namespace core::parser;

	SUBCASE("General")
	{
		auto p = parse("");
		auto ast = p.retrieveAST();
		auto root = ast->globalNode.get();

		CHECK(root->nodes.size() == 0);
		REQUIRE(!p.getError());
	}
}
