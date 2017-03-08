// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/FwdDecl.h"
#include "ast/Node.h"
#include <cereal.h>
#include <memory>
#include <vector>

namespace ast
{
/// Generic expression
class Expr : public Node
{
public:
    Expr() : Node(EXPR)
    {
    }
    Expr(const Expr&) = delete;
    Expr& operator=(const Expr&) = delete;
    Expr(Expr&&) = default;
    Expr& operator=(Expr&&) = default;
    ~Expr() override = default;

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
    explicit Expr(NodeType t) : Node(t)
    {
    }
};

/// Empty expression
class EmptyExpr : public Expr
{
public:
    EmptyExpr() : Expr(EMPTY_EXPR)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Expr>(this));
    }
};

/// Identifier expression
class IdentifierExpr : public Expr
{
    friend class cereal::access;

    IdentifierExpr() : Expr(IDENTIFIER_EXPR)
    {
    }

public:
    explicit IdentifierExpr(std::string&& val)
        : Expr(IDENTIFIER_EXPR), value(std::move(val))
    {
    }
    IdentifierExpr(const IdentifierExpr&) = delete;
    IdentifierExpr& operator=(const IdentifierExpr&) = delete;
    IdentifierExpr(IdentifierExpr&&) = default;
    IdentifierExpr& operator=(IdentifierExpr&&) = default;
    ~IdentifierExpr() override = default;

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Expr>(this), CEREAL_NVP(value));
    }

    /// Textual value of the identifier
    std::string value;

protected:
    IdentifierExpr(NodeType t, std::string&& val)
        : Expr(t), value(std::move(val))
    {
    }
};

/// Variable reference expression
class VariableRefExpr : public IdentifierExpr
{
    friend class cereal::access;

    VariableRefExpr() : IdentifierExpr(VARIABLE_REF_EXPR, "")
    {
    }

public:
    explicit VariableRefExpr(std::string&& val)
        : IdentifierExpr(VARIABLE_REF_EXPR, std::move(val))
    {
    }

    VariableRefExpr(const VariableRefExpr&) = delete;
    VariableRefExpr& operator=(const VariableRefExpr&) = delete;
    VariableRefExpr(VariableRefExpr&&) = default;
    VariableRefExpr& operator=(VariableRefExpr&&) = default;
    ~VariableRefExpr() override = default;

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<IdentifierExpr>(this));
    }
};

class VariableDefinitionExpr : public Expr
{
    friend class cereal::access;

    VariableDefinitionExpr() : Expr(VARIABLE_DEFINITION_EXPR)
    {
    }

public:
    VariableDefinitionExpr(std::unique_ptr<IdentifierExpr> pType,
                           std::unique_ptr<IdentifierExpr> pName,
                           std::unique_ptr<Expr> pInit)
        : Expr(VARIABLE_DEFINITION_EXPR), type(std::move(pType)),
          name(std::move(pName)), init(std::move(pInit)), typeInferred{false}
    {
    }
    VariableDefinitionExpr(std::unique_ptr<IdentifierExpr> pName,
                           std::unique_ptr<Expr> pInit)
        : Expr(VARIABLE_DEFINITION_EXPR),
          type(std::make_unique<IdentifierExpr>("")), name(std::move(pName)),
          init(std::move(pInit)), typeInferred{true}
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Expr>(this), CEREAL_NVP(type),
                CEREAL_NVP(name), CEREAL_NVP(init), CEREAL_NVP(typeInferred),
                CEREAL_NVP(isMutable));
    }

    std::unique_ptr<IdentifierExpr> type, name;
    std::unique_ptr<Expr> init;
    bool typeInferred{false};
    bool isMutable{false};
};

class GlobalVariableDefinitionExpr : public Expr
{
    friend class cereal::access;

    GlobalVariableDefinitionExpr() : Expr(GLOBAL_VARIABLE_DEFINITION_EXPR)
    {
    }

public:
    GlobalVariableDefinitionExpr(std::unique_ptr<IdentifierExpr> pType,
                                 std::unique_ptr<IdentifierExpr> pName,
                                 std::unique_ptr<Expr> pInit)
        : Expr(GLOBAL_VARIABLE_DEFINITION_EXPR),
          var(std::make_unique<VariableDefinitionExpr>(
              std::move(pType), std::move(pName), std::move(pInit)))
    {
    }
    GlobalVariableDefinitionExpr(std::unique_ptr<IdentifierExpr> pName,
                                 std::unique_ptr<Expr> pInit)
        : Expr(GLOBAL_VARIABLE_DEFINITION_EXPR),
          var(std::make_unique<VariableDefinitionExpr>(std::move(pName),
                                                       std::move(pInit)))
    {
    }
    GlobalVariableDefinitionExpr(std::unique_ptr<VariableDefinitionExpr> pVar)
        : Expr(GLOBAL_VARIABLE_DEFINITION_EXPR), var(std::move(pVar))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Expr>(this), CEREAL_NVP(var));
    }

    std::unique_ptr<VariableDefinitionExpr> var;
};
} // namespace ast
