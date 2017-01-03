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

#include "core/lexer/Lexer.h"
#include "util/Logger.h"

TEST_CASE("Test lexer")
{
	using namespace core::lexer;

	TokenVector v;
	std::string code;

	SUBCASE("General")
	{
		SUBCASE("Whitespace skipping")
		{
			code = " \n\t\v";
			Lexer l(code, TEST_FILE);
			v = l.run();
			CHECK(v.size() == 1);
			REQUIRE(!l.getError());
			REQUIRE(l.getErrorLevel() == ERROR_WARNING);

			REQUIRE(v[0].type == TOKEN_EOF);
		}

		SUBCASE("Comments")
		{
			code = R"(// Comment
123//Literal
			)";
			Lexer l(code, TEST_FILE);
			v = l.run();

			CHECK(v.size() == 2);
			REQUIRE(!l.getError());

			REQUIRE(v[0].type == TOKEN_LITERAL_INTEGER);
		}

	}

	SUBCASE("Literals")
	{
		SUBCASE("Integer literals")
		{
			code = "123 9999999i32 8i8 5i16 10i64 1001b 60o 0xff 0xdead";
			Lexer l(code, TEST_FILE);
			v = l.run();
			CHECK(v.size() == 10);
			REQUIRE(!l.getError());

			for(unsigned int i = 0; i <= 8; ++i)
			{
				REQUIRE(v[i].type == TOKEN_LITERAL_INTEGER);
			}

			REQUIRE(v[0].value == "123");
			REQUIRE(v[0].modifierInt.get() == INTEGER_INT);

			REQUIRE(v[1].value == "9999999");
			REQUIRE(v[1].modifierInt.isSet(INTEGER_INT32));
			REQUIRE(v[1].modifierInt.isNotSet(INTEGER_HEX));

			REQUIRE(v[2].value == "8");
			REQUIRE(v[2].modifierInt.isSet(INTEGER_INT8));

			REQUIRE(v[3].value == "5");
			REQUIRE(v[3].modifierInt.isSet(INTEGER_INT16));
			REQUIRE(v[3].modifierInt.isNotSet(INTEGER_INT64));
			REQUIRE(v[3].modifierInt.isNotSet(INTEGER_OCTAL));

			REQUIRE(v[4].value == "10");
			REQUIRE(v[4].modifierInt.isSet(INTEGER_INT64));

			REQUIRE(v[5].value == "1001");
			REQUIRE(v[5].modifierInt.isSet(INTEGER_BINARY));

			REQUIRE(v[6].value == "60");
			REQUIRE(v[6].modifierInt.isSet(INTEGER_OCTAL));

			REQUIRE(v[7].value == "ff");
			REQUIRE(v[7].modifierInt.isSet(INTEGER_HEX));
			REQUIRE(v[7].modifierInt.isNotSet(INTEGER_INT64));

			REQUIRE(v[8].value == "dead");
			REQUIRE(v[8].modifierInt.isSet(INTEGER_HEX));
		}
		SUBCASE("Float literals")
		{
			code = "3.1415926535 12.34f32 42.0f64 39.99d";
			Lexer l(code, TEST_FILE);
			v = l.run();
			CHECK(v.size() == 5);
			REQUIRE(!l.getError());

			for(unsigned int i = 0; i <= 3; ++i)
			{
				REQUIRE(v[i].type == TOKEN_LITERAL_FLOAT);
			}

			REQUIRE(v[0].value == "3.1415926535");
			REQUIRE(v[0].modifierFloat.isNotSet(FLOAT_F32));
			REQUIRE(v[0].modifierFloat.isNotSet(FLOAT_F64));
			REQUIRE(v[0].modifierFloat.isNotSet(FLOAT_DECIMAL));
			REQUIRE(v[0].modifierFloat.isSet(FLOAT_FLOAT));

			REQUIRE(v[1].value == "12.34");
			REQUIRE(v[1].modifierFloat.isSet(FLOAT_F32));
			REQUIRE(v[1].modifierFloat.isNotSet(FLOAT_F64));

			REQUIRE(v[2].value == "42.0");
			REQUIRE(v[2].modifierFloat.isSet(FLOAT_F64));
			REQUIRE(v[2].modifierFloat.isNotSet(FLOAT_FLOAT));

			REQUIRE(v[3].value == "39.99");
			REQUIRE(v[3].modifierFloat.isSet(FLOAT_DECIMAL));
		}
		SUBCASE("String literals")
		{
			code = "\"String\" \"Special\\nstring\\t\" \"Hex \\x30 Oct \\o60\"";
			Lexer l(code, TEST_FILE);
			v = l.run();
			CHECK(v.size() == 4);
			REQUIRE(!l.getError());

			for(unsigned int i = 0; i <= 2; ++i)
			{
				REQUIRE(v[i].type == TOKEN_LITERAL_STRING);
			}

			REQUIRE(v[0].value == "String");
			REQUIRE(v[1].value == "Special\nstring\t");
			REQUIRE(v[2].value == "Hex 0 Oct 0");
		}
	}

	SUBCASE("Identifiers")
	{
		code = "io.stdout.writeln(\"Hello World\")";
		Lexer l(code, TEST_FILE);
		v = l.run();
		CHECK(v.size() == 9);
		REQUIRE(!l.getError());

		REQUIRE(v[0].type == TOKEN_IDENTIFIER);
		REQUIRE(v[0].value == "io");
		REQUIRE(v[1].type == TOKEN_OPERATORB_MEMBER);

		REQUIRE(v[2].type == TOKEN_IDENTIFIER);
		REQUIRE(v[2].value == "stdout");
		REQUIRE(v[3].type == TOKEN_OPERATORB_MEMBER);

		REQUIRE(v[4].type == TOKEN_IDENTIFIER);
		REQUIRE(v[4].value == "writeln");

		REQUIRE(v[5].type == TOKEN_PUNCT_PAREN_OPEN);
		REQUIRE(v[6].type == TOKEN_LITERAL_STRING);
		REQUIRE(v[6].value == "Hello World");
		REQUIRE(v[7].type == TOKEN_PUNCT_PAREN_CLOSE);
	}

	SUBCASE("Import")
	{
		SUBCASE("Default import")
		{
			code = "import io;";
			Lexer l(code, TEST_FILE);
			v = l.run();
			CHECK(v.size() == 4);
			REQUIRE(!l.getError());

			REQUIRE(v[0].type == TOKEN_KEYWORD_IMPORT);

			REQUIRE(v[1].type == TOKEN_IDENTIFIER);
			REQUIRE(v[1].value == "io");

			REQUIRE(v[2].type == TOKEN_PUNCT_SEMICOLON);
		}

		SUBCASE("Module import")
		{
			code = "import module mymodule;";
			Lexer l(code, TEST_FILE);
			v = l.run();
			CHECK(v.size() == 5);
			REQUIRE(!l.getError());

			REQUIRE(v[0].type == TOKEN_KEYWORD_IMPORT);
			REQUIRE(v[1].type == TOKEN_KEYWORD_MODULE);

			REQUIRE(v[2].type == TOKEN_IDENTIFIER);
			REQUIRE(v[2].value == "mymodule");

			REQUIRE(v[3].type == TOKEN_PUNCT_SEMICOLON);
		}

		SUBCASE("Package import")
		{
			code = "import package cstd.io;";
			Lexer l(code, TEST_FILE);
			v = l.run();
			CHECK(v.size() == 7);
			REQUIRE(!l.getError());

			REQUIRE(v[0].type == TOKEN_KEYWORD_IMPORT);
			REQUIRE(v[1].type == TOKEN_KEYWORD_PACKAGE);

			REQUIRE(v[2].type == TOKEN_IDENTIFIER);
			REQUIRE(v[2].value == "cstd");
			REQUIRE(v[3].type == TOKEN_OPERATORB_MEMBER);
			REQUIRE(v[4].type == TOKEN_IDENTIFIER);
			REQUIRE(v[4].value == "io");

			REQUIRE(v[5].type == TOKEN_PUNCT_SEMICOLON);
		}

		SUBCASE("Path import")
		{
			code = "import module \"path/to/module\";";
			Lexer l(code, TEST_FILE);
			v = l.run();
			CHECK(v.size() == 5);
			REQUIRE(!l.getError());

			REQUIRE(v[0].type == TOKEN_KEYWORD_IMPORT);
			REQUIRE(v[1].type == TOKEN_KEYWORD_MODULE);

			REQUIRE(v[2].type == TOKEN_LITERAL_STRING);
			REQUIRE(v[2].value == "path/to/module");

			REQUIRE(v[3].type == TOKEN_PUNCT_SEMICOLON);
		}
	}
	SUBCASE("Main function definition")
	{
		code = "def main(List<String>): Void {}";
		Lexer l(code, TEST_FILE);
		v = l.run();
		CHECK(v.size() == 13);
		REQUIRE(!l.getError());

		REQUIRE(v[0].type == TOKEN_KEYWORD_DEFINE);

		REQUIRE(v[1].type == TOKEN_IDENTIFIER);
		REQUIRE(v[1].value == "main");

		REQUIRE(v[2].type == TOKEN_PUNCT_PAREN_OPEN);
		REQUIRE(v[3].type == TOKEN_IDENTIFIER);
		REQUIRE(v[3].value == "List");
		REQUIRE(v[4].type == TOKEN_OPERATORB_LESS);
		REQUIRE(v[5].type == TOKEN_IDENTIFIER);
		REQUIRE(v[5].value == "String");
		REQUIRE(v[6].type == TOKEN_OPERATORB_GREATER);
		REQUIRE(v[7].type == TOKEN_PUNCT_PAREN_CLOSE);

		REQUIRE(v[8].type == TOKEN_PUNCT_COLON);
		REQUIRE(v[9].type == TOKEN_IDENTIFIER);
		REQUIRE(v[9].value == "Void");

		REQUIRE(v[10].type == TOKEN_PUNCT_BRACE_OPEN);
		REQUIRE(v[11].type == TOKEN_PUNCT_BRACE_CLOSE);
	}
	SUBCASE("If-else")
	{
		code = "if(foo == 123) bar = true; else bar = false;";
		Lexer l(code, TEST_FILE);
		v = l.run();
		CHECK(v.size() == 16);
		REQUIRE(!l.getError());

		REQUIRE(v[0].type == TOKEN_KEYWORD_IF);
		REQUIRE(v[1].type == TOKEN_PUNCT_PAREN_OPEN);
		REQUIRE(v[2].type == TOKEN_IDENTIFIER);
		REQUIRE(v[2].value == "foo");
		REQUIRE(v[3].type == TOKEN_OPERATORB_EQ);
		REQUIRE(v[4].type == TOKEN_LITERAL_INTEGER);
		REQUIRE(v[4].value == "123");
		REQUIRE(v[5].type == TOKEN_PUNCT_PAREN_CLOSE);

		REQUIRE(v[6].type == TOKEN_IDENTIFIER);
		REQUIRE(v[6].value == "bar");
		REQUIRE(v[7].type == TOKEN_OPERATORA_SIMPLE);
		REQUIRE(v[8].type == TOKEN_LITERAL_TRUE);
		REQUIRE(v[9].type == TOKEN_PUNCT_SEMICOLON);

		REQUIRE(v[10].type == TOKEN_KEYWORD_ELSE);

		REQUIRE(v[11].type == TOKEN_IDENTIFIER);
		REQUIRE(v[11].value == "bar");
		REQUIRE(v[12].type == TOKEN_OPERATORA_SIMPLE);
		REQUIRE(v[13].type == TOKEN_LITERAL_FALSE);
		REQUIRE(v[14].type == TOKEN_PUNCT_SEMICOLON);
	}
}
