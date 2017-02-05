// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/lexer/Lexer.h"
#include "core/parser/Parser.h"
#include "util/File.h"
#include "util/Logger.h"
#include <doctest.h>

static auto getFile()
{
    static auto f = std::make_shared<util::File>(TEST_FILE);
    return f;
}

static auto runLexer(const std::string& code)
{
    using namespace core::lexer;

    getFile()->setContent(code);
    Lexer l(getFile());
    return l.run();
}

static auto parse(const std::string& code)
{
    using namespace core::parser;

    Parser p(getFile(), runLexer(code));
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
        CHECK(!p.getError());
    }
}
