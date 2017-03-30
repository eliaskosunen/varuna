// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "ast/FwdDecl.h"
#include <spdlog.h>
#include <ostream>

namespace ast
{
/// Serialize the AST to desired format
class Serializer
{
public:
    /// Serialization output format
    enum SerializeType
    {
        BIN,
        XML,
        JSON
    };

    explicit Serializer(std::shared_ptr<AST> a);

    /**
     * Serialize the AST and write outputs to a std::ostream
     * @param os   Stream to write outputs to
     * @param type Output format
     */
    void run(std::ostream& os, SerializeType type = XML);
    /**
     * Serialize the AST and write outputs to a log
     * @param logger Logger to write outputs to
     * @param level  Logging level
     * @param type   Output format
     */
    void run(spdlog::logger& logger, spdlog::level::level_enum level,
             SerializeType type = XML);

    /// Dump the AST to stdout using DumpVisitor
    void runDump();

private:
    std::shared_ptr<AST> ast;
};
} // namespace ast

// CEREAL_FORCE_DYNAMIC_INIT(ast)
