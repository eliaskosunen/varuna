// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTExpression.h"
#include "ast/ASTNode.h"
#include "ast/FwdDecl.h"
#include <memory>
#include <vector>

namespace ast
{
/// Generic statement
class ASTStatement : public ASTNode
{
public:
    ASTStatement() : ASTNode(STMT)
    {
    }
    ASTStatement(const ASTStatement&) = delete;
    ASTStatement& operator=(const ASTStatement&) = delete;
    ASTStatement(ASTStatement&&) = default;
    ASTStatement& operator=(ASTStatement&&) = default;
    ~ASTStatement() override = default;

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    virtual std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v);
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

protected:
    explicit ASTStatement(NodeType t) : ASTNode(t)
    {
    }
};

/// Empty statement
class ASTEmptyStatement : public ASTStatement
{
public:
    ASTEmptyStatement() : ASTStatement(EMPTY_STMT)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;
};

/// Block statement
class ASTBlockStatement : public ASTStatement
{
public:
    ASTBlockStatement() : ASTStatement(BLOCK_STMT)
    {
    }
    explicit ASTBlockStatement(std::unique_ptr<ASTStatement> first)
        : ASTStatement(BLOCK_STMT)
    {
        nodes.push_back(std::move(first));
    }
    explicit ASTBlockStatement(std::vector<std::unique_ptr<ASTStatement>>&& vec)
        : ASTStatement(BLOCK_STMT), nodes(std::move(vec))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Node list
    std::vector<std::unique_ptr<ASTStatement>> nodes;
};

/// Wrapped expression/expression statement
class ASTWrappedExpressionStatement : public ASTStatement
{
public:
    explicit ASTWrappedExpressionStatement(std::unique_ptr<ASTExpression> pExpr)
        : ASTStatement(WRAPPED_EXPR_STMT), expr(std::move(pExpr))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Expression inside the statement
    std::unique_ptr<ASTExpression> expr;
};
} // namespace ast
