// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "codegen/Codegen.h"

namespace codegen
{
class Generator
{
public:
    using GeneratorClass = Codegen;

    Generator(std::shared_ptr<ast::AST> t);

    bool run();

    std::unique_ptr<GeneratorClass> get();

    constexpr const char* getIdentifier() const noexcept
    {
        return "Varuna Compiler Code Generator";
    }

private:
    std::shared_ptr<ast::AST> ast;
    std::unique_ptr<GeneratorClass> c;
};
}
