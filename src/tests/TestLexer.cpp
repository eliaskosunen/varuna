// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/lexer/Lexer.h"
#include "util/File.h"
#include "util/Logger.h"
#include <doctest.h>

TEST_CASE("Test lexer")
{
    using namespace core::lexer;

    TokenVector v;
    util::File f(TEST_FILE);

    SUBCASE("General")
    {
        SUBCASE("Whitespace skipping")
        {
            f.setContent(" \n\t\v");
            Lexer l(&f);
            v = l.run();
            CHECK(v.size() == 1);
            CHECK(!l.getError());
            CHECK(l.getErrorLevel() == ERROR_WARNING);

            CHECK(v[0].type == TOKEN_EOF);
        }

        SUBCASE("Comments")
        {
            f.setContent(R"(// Comment
123//Literal
			)");
            Lexer l(&f);
            v = l.run();

            CHECK(v.size() == 2);
            CHECK(!l.getError());

            CHECK(v[0].type == TOKEN_LITERAL_INTEGER);
        }

        SUBCASE("Multi-line comments")
        {
            f.setContent(R"(
                /*
                // Comment
                 */
                123
                /*
                /*
                Nested
                */
                 */
            )");
            Lexer l(&f);
            v = l.run();

            CHECK(v.size() == 2);
            CHECK(!l.getError());

            CHECK(v[0].type == TOKEN_LITERAL_INTEGER);
        }
    }

    SUBCASE("Line counting")
    {
        SUBCASE("'LF' line endings")
        {
            f.setContent("// Line 1\n"
                         " 123 /* Line 2 */\n"
                         " \"abc\" // Line 3\n"
                         "\n"
                         "// Comment on line 5\n"
                         "// Another comment on line 6\n"
                         "\n"
                         "foo // Line 8\n");
            Lexer l(&f);
            v = l.run();

            CHECK(v.size() == 4);
            CHECK(!l.getError());

            CHECK(v[0].loc.line == 2);
            CHECK(v[1].loc.line == 3);
            CHECK(v[2].loc.line == 8);
        }

        SUBCASE("'CR' line endings")
        {
            // CR line endings cause warnings by design
            f.setContent("// Line 1\r"
                         " 123 /* Line 2 */\r"
                         " \"abc\" // Line 3\r"
                         "\r"
                         "// Comment on line 5\r"
                         "// Another comment on line 6\r"
                         "\r"
                         "foo // Line 8\r");
            Lexer l(&f);
            v = l.run();

            CHECK(v.size() == 4);
            CHECK(!l.getError());

            CHECK(v[0].loc.line == 2);
            CHECK(v[1].loc.line == 3);
            CHECK(v[2].loc.line == 8);
        }

        SUBCASE("'CRLF' line endings")
        {
            f.setContent("// Line 1\r\n"
                         " 123 /* Line 2 */\r\n"
                         " \"abc\" // Line 3\r\n"
                         "\r\n"
                         "// Comment on line 5\r\n"
                         "// Another comment on line 6\r\n"
                         "\r\n"
                         "foo // Line 8\r\n");
            Lexer l(&f);
            v = l.run();

            CHECK(v.size() == 4);
            CHECK(!l.getError());

            CHECK(v[0].loc.line == 2);
            CHECK(v[1].loc.line == 3);
            CHECK(v[2].loc.line == 8);
        }

        SUBCASE("Mixed line endings")
        {
            f.setContent("// Line 1\r\n"
                         " 123 /* Line 2 */\r"
                         " \"abc\" // Line 3\n"
                         "\r\n"
                         "// Comment on line 5\n"
                         "// Another comment on line 6\n"
                         "\r\n"
                         "foo // Line 8\n");
            Lexer l(&f);
            v = l.run();

            CHECK(v.size() == 4);
            CHECK(!l.getError());

            CHECK(v[0].loc.line == 2);
            CHECK(v[1].loc.line == 3);
            CHECK(v[2].loc.line == 8);
        }
    }

    SUBCASE("Literals")
    {
        SUBCASE("Integer literals")
        {
            f.setContent(
                "123 9999999i32 8i8 5i16 10i64 0b1001 0o60 0XFF 0xdead 0x6bo");
            Lexer l(&f);
            v = l.run();
            CHECK(v.size() == 11);
            CHECK(!l.getError());

            for(unsigned int i = 0; i <= 9; ++i)
            {
                CHECK(v[i].type == TOKEN_LITERAL_INTEGER);
            }

            CHECK(v[0].value == "123");
            CHECK(v[0].modifierInt.get() == INTEGER_INT);

            CHECK(v[1].value == "9999999");
            CHECK(v[1].modifierInt.isSet(INTEGER_INT32));
            CHECK(v[1].modifierInt.isNotSet(INTEGER_HEX));

            CHECK(v[2].value == "8");
            CHECK(v[2].modifierInt.isSet(INTEGER_INT8));

            CHECK(v[3].value == "5");
            CHECK(v[3].modifierInt.isSet(INTEGER_INT16));
            CHECK(v[3].modifierInt.isNotSet(INTEGER_INT64));
            CHECK(v[3].modifierInt.isNotSet(INTEGER_OCT));

            CHECK(v[4].value == "10");
            CHECK(v[4].modifierInt.isSet(INTEGER_INT64));

            CHECK(v[5].value == "1001");
            CHECK(v[5].modifierInt.isSet(INTEGER_BIN));

            CHECK(v[6].value == "60");
            CHECK(v[6].modifierInt.isSet(INTEGER_OCT));

            CHECK(v[7].value == "FF");
            CHECK(v[7].modifierInt.isSet(INTEGER_HEX));
            CHECK(v[7].modifierInt.isNotSet(INTEGER_INT64));

            CHECK(v[8].value == "dead");
            CHECK(v[8].modifierInt.isSet(INTEGER_HEX));

            CHECK(v[9].value == "6b");
            CHECK(v[9].modifierInt.isSet(INTEGER_HEX));
            CHECK(v[9].modifierInt.isSet(INTEGER_BYTE));
        }
        SUBCASE("Float literals")
        {
            f.setContent("3.1415926535 12.34f32 42.0f64 39.99d");
            Lexer l(&f);
            v = l.run();
            CHECK(v.size() == 5);
            CHECK(!l.getError());

            for(unsigned int i = 0; i <= 3; ++i)
            {
                CHECK(v[i].type == TOKEN_LITERAL_FLOAT);
            }

            CHECK(v[0].value == "3.1415926535");
            CHECK(v[0].modifierFloat.isNotSet(FLOAT_F32));
            CHECK(v[0].modifierFloat.isNotSet(FLOAT_F64));
            CHECK(v[0].modifierFloat.isNotSet(FLOAT_DECIMAL));
            CHECK(v[0].modifierFloat.isSet(FLOAT_FLOAT));

            CHECK(v[1].value == "12.34");
            CHECK(v[1].modifierFloat.isSet(FLOAT_F32));
            CHECK(v[1].modifierFloat.isNotSet(FLOAT_F64));

            CHECK(v[2].value == "42.0");
            CHECK(v[2].modifierFloat.isSet(FLOAT_F64));
            CHECK(v[2].modifierFloat.isNotSet(FLOAT_FLOAT));

            CHECK(v[3].value == "39.99");
            CHECK(v[3].modifierFloat.isSet(FLOAT_DECIMAL));
        }
        SUBCASE("String literals")
        {
            f.setContent(
                "\"String\" \"Special\\nstring\\t\" \"Hex \\x30 Oct \\o60\"");
            Lexer l(&f);
            v = l.run();
            CHECK(v.size() == 4);
            CHECK(!l.getError());

            for(unsigned int i = 0; i <= 2; ++i)
            {
                CHECK(v[i].type == TOKEN_LITERAL_STRING);
            }

            CHECK(v[0].value == "String");
            CHECK(v[1].value == "Special\nstring\t");
            CHECK(v[2].value == "Hex 0 Oct 0");
        }
    }

    SUBCASE("Identifiers")
    {
        f.setContent("io.stdout.writeln(\"Hello World\")");
        Lexer l(&f);
        v = l.run();
        CHECK(v.size() == 9);
        CHECK(!l.getError());

        CHECK(v[0].type == TOKEN_IDENTIFIER);
        CHECK(v[0].value == "io");
        CHECK(v[1].type == TOKEN_OPERATORB_MEMBER);

        CHECK(v[2].type == TOKEN_IDENTIFIER);
        CHECK(v[2].value == "stdout");
        CHECK(v[3].type == TOKEN_OPERATORB_MEMBER);

        CHECK(v[4].type == TOKEN_IDENTIFIER);
        CHECK(v[4].value == "writeln");

        CHECK(v[5].type == TOKEN_PUNCT_PAREN_OPEN);
        CHECK(v[6].type == TOKEN_LITERAL_STRING);
        CHECK(v[6].value == "Hello World");
        CHECK(v[7].type == TOKEN_PUNCT_PAREN_CLOSE);
    }

    SUBCASE("Import")
    {
        SUBCASE("Default import")
        {
            f.setContent("import io;");
            Lexer l(&f);
            v = l.run();
            CHECK(v.size() == 4);
            CHECK(!l.getError());

            CHECK(v[0].type == TOKEN_KEYWORD_IMPORT);

            CHECK(v[1].type == TOKEN_IDENTIFIER);
            CHECK(v[1].value == "io");

            CHECK(v[2].type == TOKEN_PUNCT_SEMICOLON);
        }

        SUBCASE("Module import")
        {
            f.setContent("import module mymodule;");
            Lexer l(&f);
            v = l.run();
            CHECK(v.size() == 5);
            CHECK(!l.getError());

            CHECK(v[0].type == TOKEN_KEYWORD_IMPORT);
            CHECK(v[1].type == TOKEN_KEYWORD_MODULE);

            CHECK(v[2].type == TOKEN_IDENTIFIER);
            CHECK(v[2].value == "mymodule");

            CHECK(v[3].type == TOKEN_PUNCT_SEMICOLON);
        }

        SUBCASE("Package import")
        {
            f.setContent("import package cstd.io;");
            Lexer l(&f);
            v = l.run();
            CHECK(v.size() == 7);
            CHECK(!l.getError());

            CHECK(v[0].type == TOKEN_KEYWORD_IMPORT);
            CHECK(v[1].type == TOKEN_KEYWORD_PACKAGE);

            CHECK(v[2].type == TOKEN_IDENTIFIER);
            CHECK(v[2].value == "cstd");
            CHECK(v[3].type == TOKEN_OPERATORB_MEMBER);
            CHECK(v[4].type == TOKEN_IDENTIFIER);
            CHECK(v[4].value == "io");

            CHECK(v[5].type == TOKEN_PUNCT_SEMICOLON);
        }

        SUBCASE("Path import")
        {
            f.setContent("import module \"path/to/module\";");
            Lexer l(&f);
            v = l.run();
            CHECK(v.size() == 5);
            CHECK(!l.getError());

            CHECK(v[0].type == TOKEN_KEYWORD_IMPORT);
            CHECK(v[1].type == TOKEN_KEYWORD_MODULE);

            CHECK(v[2].type == TOKEN_LITERAL_STRING);
            CHECK(v[2].value == "path/to/module");

            CHECK(v[3].type == TOKEN_PUNCT_SEMICOLON);
        }
    }
    SUBCASE("Main function definition")
    {
        f.setContent("def main(): int {}");
        Lexer l(&f);
        v = l.run();
        CHECK(v.size() == 9);
        CHECK(!l.getError());

        CHECK(v[0].type == TOKEN_KEYWORD_DEFINE);

        CHECK(v[1].type == TOKEN_IDENTIFIER);
        CHECK(v[1].value == "main");

        CHECK(v[2].type == TOKEN_PUNCT_PAREN_OPEN);
        CHECK(v[3].type == TOKEN_PUNCT_PAREN_CLOSE);

        CHECK(v[4].type == TOKEN_PUNCT_COLON);
        CHECK(v[5].type == TOKEN_IDENTIFIER);
        CHECK(v[5].value == "int");

        CHECK(v[6].type == TOKEN_PUNCT_BRACE_OPEN);
        CHECK(v[7].type == TOKEN_PUNCT_BRACE_CLOSE);
    }
    SUBCASE("If-else")
    {
        f.setContent("if(foo == 123) bar = true; else bar = false;");
        Lexer l(&f);
        v = l.run();
        CHECK(v.size() == 16);
        CHECK(!l.getError());

        CHECK(v[0].type == TOKEN_KEYWORD_IF);
        CHECK(v[1].type == TOKEN_PUNCT_PAREN_OPEN);
        CHECK(v[2].type == TOKEN_IDENTIFIER);
        CHECK(v[2].value == "foo");
        CHECK(v[3].type == TOKEN_OPERATORB_EQ);
        CHECK(v[4].type == TOKEN_LITERAL_INTEGER);
        CHECK(v[4].value == "123");
        CHECK(v[5].type == TOKEN_PUNCT_PAREN_CLOSE);

        CHECK(v[6].type == TOKEN_IDENTIFIER);
        CHECK(v[6].value == "bar");
        CHECK(v[7].type == TOKEN_OPERATORA_SIMPLE);
        CHECK(v[8].type == TOKEN_LITERAL_TRUE);
        CHECK(v[9].type == TOKEN_PUNCT_SEMICOLON);

        CHECK(v[10].type == TOKEN_KEYWORD_ELSE);

        CHECK(v[11].type == TOKEN_IDENTIFIER);
        CHECK(v[11].value == "bar");
        CHECK(v[12].type == TOKEN_OPERATORA_SIMPLE);
        CHECK(v[13].type == TOKEN_LITERAL_FALSE);
        CHECK(v[14].type == TOKEN_PUNCT_SEMICOLON);
    }
}
