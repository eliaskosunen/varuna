// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/Frontend.h"
#include "ast/Serializer.h"

namespace core
{
Frontend::Frontend(std::shared_ptr<util::File> f)
    : file(std::move(f)), ast{nullptr}
{
    assert(file);
}

std::shared_ptr<ast::AST> Frontend::run()
{
    if(!runLexer())
    {
        return nullptr;
    }
    if(!runParser())
    {
        return nullptr;
    }
    assert(ast);
    return std::shared_ptr<ast::AST>(std::move(ast));
}

bool Frontend::runLexer()
{
    util::logger->debug("Starting lexer");
    lexer::Lexer l(file);
    tokens = l.run();
    if(l.getError())
    {
        util::logger->debug("Lexing failed");
        util::logger->info("Lexing of file '{}' failed, terminating\n",
                           file->getFilename());
        return false;
    }
    util::logger->debug("Lexing finished\n");
    return true;
}

bool Frontend::runParser()
{
    util::logger->debug("Starting parser");
    parser::Parser p(file, tokens);
    p.run();
    if(p.getError())
    {
        util::logger->debug("Parsing failed");
        util::logger->info("Parsing of file '{}' failed, terminating\n",
                           file->getFilename());
        return false;
    }
    auto astUniq = p.retrieveAST();
    ast = std::shared_ptr<ast::AST>(std::move(astUniq));
    util::logger->debug("Parsing finished\n");

    {
        ast::Serializer s(ast);
        s.runDump();
    }

    return true;
}
} // namespace core
