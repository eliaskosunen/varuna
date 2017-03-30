// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/Expr.h"
#include "ast/FwdDecl.h"
#include "ast/Node.h"
#include <cereal.h>
#include <memory>
#include <vector>

namespace ast
{
/// Generic statement
class Stmt : public Node
{
public:
    Stmt() : Node(STMT)
    {
    }
    Stmt(const Stmt&) = delete;
    Stmt& operator=(const Stmt&) = delete;
    Stmt(Stmt&&) noexcept = default;
    Stmt& operator=(Stmt&&) noexcept = default;
    ~Stmt() override = default;

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Node>(this));
    }

protected:
    explicit Stmt(NodeType t) : Node(t)
    {
    }
};

/// Empty statement
class EmptyStmt : public Stmt
{
public:
    EmptyStmt() : Stmt(EMPTY_STMT)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this));
    }
};

/// Block statement
class BlockStmt : public Stmt
{
public:
    BlockStmt() : Stmt(BLOCK_STMT)
    {
    }
    explicit BlockStmt(std::unique_ptr<Stmt> first) : Stmt(BLOCK_STMT)
    {
        nodes.push_back(std::move(first));
    }
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>>&& vec)
        : Stmt(BLOCK_STMT), nodes(std::move(vec))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(nodes));
    }

    /// Node list
    std::vector<std::unique_ptr<Stmt>> nodes;
};

/// Wrapped expression/expression statement
class ExprStmt : public Stmt
{
    friend class cereal::access;

    ExprStmt() : Stmt(EXPR_STMT)
    {
    }

public:
    explicit ExprStmt(std::unique_ptr<Expr> pExpr)
        : Stmt(EXPR_STMT), expr(std::move(pExpr))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(expr));
    }

    /// Expression inside the statement
    std::unique_ptr<Expr> expr;
};

/// Aliasing statement
class AliasStmt : public Stmt
{
    friend class cereal::access;

    AliasStmt() : Stmt(ALIAS_STMT)
    {
    }

public:
    AliasStmt(std::unique_ptr<IdentifierExpr> pAlias,
              std::unique_ptr<IdentifierExpr> pAliasee)
        : Stmt(ALIAS_STMT), alias(std::move(pAlias)),
          aliasee(std::move(pAliasee))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(alias),
                CEREAL_NVP(aliasee));
    }

    std::unique_ptr<IdentifierExpr> alias;
    std::unique_ptr<IdentifierExpr> aliasee;
};
} // namespace ast
