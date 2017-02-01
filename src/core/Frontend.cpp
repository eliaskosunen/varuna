// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/Frontend.h"

namespace core
{
Frontend::Frontend(std::shared_ptr<util::File> f)
    : file(f), tokens{}, ast{nullptr}
{
    assert(file);
}

std::unique_ptr<ast::AST> Frontend::run()
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
    return std::move(ast);
}

bool Frontend::runLexer()
{
    util::logger->debug("Starting lexer");
    lexer::Lexer l(file.get());
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
    parser::Parser p(tokens);
    p.run();
    if(p.getError())
    {
        util::logger->debug("Parsing failed");
        util::logger->info("Parsing of file '{}' failed, terminating\n",
                           tokens[0].loc.file);
        return false;
    }
    ast = p.retrieveAST();
    util::logger->debug("Parsing finished\n");

    ast::DumpASTVisitor::dump<ast::ASTBlockStatement>(ast->globalNode.get());
    return true;
}
} // namespace core
