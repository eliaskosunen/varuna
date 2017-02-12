// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTNode.h"
#include "ast/FwdDecl.h"
#include <memory>
#include <vector>

namespace ast
{
/// Generic expression
class ASTExpression : public ASTNode
{
public:
    ASTExpression() : ASTNode(EXPR)
    {
    }
    ASTExpression(const ASTExpression&) = delete;
    ASTExpression& operator=(const ASTExpression&) = delete;
    ASTExpression(ASTExpression&&) = default;
    ASTExpression& operator=(ASTExpression&&) = default;
    ~ASTExpression() override = default;

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

protected:
    explicit ASTExpression(NodeType t) : ASTNode(t)
    {
    }
};

/// Empty expression
class ASTEmptyExpression : public ASTExpression
{
public:
    ASTEmptyExpression() : ASTExpression(EMPTY_EXPR)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;
};

/// Identifier expression
class ASTIdentifierExpression : public ASTExpression
{
public:
    explicit ASTIdentifierExpression(std::string&& val)
        : ASTExpression(IDENTIFIER_EXPR), value(std::move(val))
    {
    }
    ASTIdentifierExpression(const ASTIdentifierExpression&) = delete;
    ASTIdentifierExpression& operator=(const ASTIdentifierExpression&) = delete;
    ASTIdentifierExpression(ASTIdentifierExpression&&) = default;
    ASTIdentifierExpression& operator=(ASTIdentifierExpression&&) = default;
    ~ASTIdentifierExpression() override = default;

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Textual value of the identifier
    std::string value;

protected:
    ASTIdentifierExpression(NodeType t, std::string val)
        : ASTExpression(t), value(std::move(val))
    {
    }
};

/// Variable reference expression
class ASTVariableRefExpression : public ASTIdentifierExpression
{
public:
    explicit ASTVariableRefExpression(std::string&& val)
        : ASTIdentifierExpression(VARIABLE_REF_EXPR, std::move(val))
    {
    }

    ASTVariableRefExpression(const ASTVariableRefExpression&) = delete;
    ASTVariableRefExpression&
    operator=(const ASTVariableRefExpression&) = delete;
    ASTVariableRefExpression(ASTVariableRefExpression&&) = default;
    ASTVariableRefExpression& operator=(ASTVariableRefExpression&&) = default;
    ~ASTVariableRefExpression() override = default;

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;
};

class ASTCastExpression : public ASTExpression
{
public:
    ASTCastExpression(std::unique_ptr<ASTExpression> pCastee,
                      std::unique_ptr<ASTIdentifierExpression> pType)
        : ASTExpression(CAST_EXPR), type(std::move(pType)),
          castee(std::move(pCastee))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTIdentifierExpression> type;
    std::unique_ptr<ASTExpression> castee;
};

class ASTVariableDefinitionExpression : public ASTExpression
{
public:
    ASTVariableDefinitionExpression(
        std::unique_ptr<ASTIdentifierExpression> pType,
        std::unique_ptr<ASTIdentifierExpression> pName,
        std::unique_ptr<ASTExpression> pInit)
        : ASTExpression(VARIABLE_DEFINITION_EXPR), type(std::move(pType)),
          name(std::move(pName)), init(std::move(pInit)), typeInferred{false}
    {
    }
    ASTVariableDefinitionExpression(
        std::unique_ptr<ASTIdentifierExpression> pName,
        std::unique_ptr<ASTExpression> pInit)
        : ASTExpression(VARIABLE_DEFINITION_EXPR),
          type(std::make_unique<ASTIdentifierExpression>("")),
          name(std::move(pName)), init(std::move(pInit)), typeInferred{true}
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTIdentifierExpression> type, name;
    std::unique_ptr<ASTExpression> init;
    bool typeInferred;
    bool isMutable{false};
};

class ASTGlobalVariableDefinitionExpression : public ASTExpression
{
public:
    ASTGlobalVariableDefinitionExpression(
        std::unique_ptr<ASTIdentifierExpression> pType,
        std::unique_ptr<ASTIdentifierExpression> pName,
        std::unique_ptr<ASTExpression> pInit)
        : ASTExpression(GLOBAL_VARIABLE_DEFINITION_EXPR),
          var(std::make_unique<ASTVariableDefinitionExpression>(
              std::move(pType), std::move(pName), std::move(pInit)))
    {
    }
    ASTGlobalVariableDefinitionExpression(
        std::unique_ptr<ASTIdentifierExpression> pName,
        std::unique_ptr<ASTExpression> pInit)
        : ASTExpression(GLOBAL_VARIABLE_DEFINITION_EXPR),
          var(std::make_unique<ASTVariableDefinitionExpression>(
              std::move(pName), std::move(pInit)))
    {
    }
    ASTGlobalVariableDefinitionExpression(
        std::unique_ptr<ASTVariableDefinitionExpression> pVar)
        : ASTExpression(GLOBAL_VARIABLE_DEFINITION_EXPR), var(std::move(pVar))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTVariableDefinitionExpression> var;
};

class ASTSubscriptExpression : public ASTExpression
{
protected:
    ASTSubscriptExpression(NodeType t, std::unique_ptr<ASTExpression> lhs_,
                           std::unique_ptr<ASTExpression> index_)
        : ASTExpression(t), lhs(std::move(lhs_)), index(std::move(index_))
    {
    }

public:
    ASTSubscriptExpression(std::unique_ptr<ASTExpression> pLhs,
                           std::unique_ptr<ASTExpression> pIndex)
        : ASTExpression(SUBSCRIPT_EXPR), lhs(std::move(pLhs)),
          index(std::move(pIndex))
    {
    }

    ASTSubscriptExpression(const ASTSubscriptExpression&) = delete;
    ASTSubscriptExpression(ASTSubscriptExpression&&) = default;
    ASTSubscriptExpression& operator=(const ASTSubscriptExpression&) = delete;
    ASTSubscriptExpression& operator=(ASTSubscriptExpression&&) = default;
    ~ASTSubscriptExpression() override = default;

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTExpression> lhs;
    std::unique_ptr<ASTExpression> index;
};

class ASTSubscriptRangedExpression : public ASTSubscriptExpression
{
public:
    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;
};

class ASTMemberAccessExpression : public ASTExpression
{
public:
    ASTMemberAccessExpression(std::unique_ptr<ASTExpression> l,
                              std::unique_ptr<ASTExpression> r)
        : ASTExpression(MEMBER_ACCESS_EXPR), lhs(std::move(l)),
          rhs(std::move(r))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTExpression> lhs, rhs;
};
} // namespace ast
