// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "codegen/Codegen.h"

namespace codegen
{
/// An abstraction layer over code generation classes
class Generator
{
public:
    using GeneratorClass = Codegen;

    explicit Generator(std::shared_ptr<ast::AST> t);

    /**
     * Run code generation
     * \return Success
     */
    bool run();

    /**
     * Consume the code generator class
     * \return Generator class
     */
    std::unique_ptr<GeneratorClass> get();

    /**
     * Get generator identifier
     * \return Identifier
     */
    const char* getIdentifier() const noexcept
    {
        return "Varuna Compiler Code Generator";
    }

private:
    /// AST
    std::shared_ptr<ast::AST> ast;
    /// Code generator
    std::unique_ptr<GeneratorClass> c;
};
} // namespace codegen
