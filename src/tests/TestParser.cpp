// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/lexer/Lexer.h"
#include "core/parser/Parser.h"
#include "util/File.h"
#include "util/Logger.h"
#include <doctest.h>

static core::lexer::TokenVector runLexer(const std::string& code)
{
    using namespace core::lexer;

    util::File f(TEST_FILE);
    f.content = code;
    Lexer l(&f);
    return l.run();
}

static core::parser::Parser parse(const std::string& code)
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
