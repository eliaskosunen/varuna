// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "core/lexer/Lexer.h"
#include "core/parser/Parser.h"
#include "util/File.h"

namespace core
{
class Frontend
{
public:
    explicit Frontend(std::shared_ptr<util::File> f);

    std::shared_ptr<ast::AST> run();

    const char* getIdentifier() const noexcept
    {
        return "Varuna Compiler Frontend";
    }

private:
    bool runLexer();
    bool runParser();

    std::shared_ptr<util::File> file;
    core::lexer::TokenVector tokens;
    std::shared_ptr<ast::AST> ast;
};
} // namespace core
