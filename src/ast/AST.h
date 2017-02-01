// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"
#include "ast/Visitor.h"
#include "util/Compatibility.h"
#include <algorithm>

namespace ast
{
/// AST representation
class AST final
{
public:
    explicit AST(std::string f)
        : globalNode(std::make_unique<ASTBlockStatement>()), file(std::move(f))
    {
    }

    /// Push a statement in the root node of the AST
    void pushStatement(std::unique_ptr<ASTStatement> stmt)
    {
        globalNode->nodes.push_back(std::move(stmt));
    }

    /// Wrap the expression and push it in the AST
    void pushExpression(std::unique_ptr<ASTExpression> expr)
    {
        auto stmt =
            std::make_unique<ASTWrappedExpressionStatement>(std::move(expr));
        pushStatement(std::move(stmt));
    }

    /// Get root node count
    size_t countTopLevelNodes()
    {
        return globalNode->nodes.size();
    }

    std::unique_ptr<ASTBlockStatement> globalNode;
    std::string file;
};
} // namespace ast
