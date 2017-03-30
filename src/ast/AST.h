// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/FwdDecl.h"
#include "ast/Stmt.h"
#include "ast/Visitor.h"
#include "util/File.h"
#include <algorithm>

namespace ast
{
/// AST representation
class AST final
{
    friend class cereal::access;

    AST() = default;

public:
    /**
     * AST constructor
     * @param  f Source file of AST
     */
    explicit AST(std::shared_ptr<util::File> f)
        : globalNode(std::make_unique<BlockStmt>()), file(std::move(f))
    {
    }

    /**
     * Push a statement to the global node list
     * @param node Node to push
     */
    void push(std::unique_ptr<Stmt> node)
    {
        globalNode->nodes.push_back(std::move(node));
    }

    /**
     * Get the amount of global nodes
     * @return Global node count
     */
    size_t countTopLevelNodes()
    {
        return globalNode->nodes.size();
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(file), CEREAL_NVP(globalNode));
    }

    /// Global node
    std::unique_ptr<BlockStmt> globalNode;
    /// Source file of the tree
    std::shared_ptr<util::File> file;
};
} // namespace ast
