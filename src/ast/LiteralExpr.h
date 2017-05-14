// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/Expr.h"
#include "ast/FwdDecl.h"
#include "util/SafeEnum.h"
#include <cereal.h>

namespace ast
{
/// Integer literal
class IntegerLiteralExpr : public Expr
{
    friend class cereal::access;

    IntegerLiteralExpr() : Expr(INTEGER_LITERAL_EXPR)
    {
    }

public:
    IntegerLiteralExpr(int64_t val, std::unique_ptr<IdentifierExpr> pType,
                       bool pIsSigned = true)
        : Expr(INTEGER_LITERAL_EXPR), type(std::move(pType)), value(val),
          isSigned(pIsSigned)
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
                CEREAL_NVP(value), CEREAL_NVP(isSigned));
    }

    /// Type of the literal
    std::unique_ptr<IdentifierExpr> type;
    /// Value of the literal
    int64_t value{0};
    /// Is a signed integer
    bool isSigned{true};
};

/// Floating-point literal
class FloatLiteralExpr : public Expr
{
    friend class cereal::access;

    FloatLiteralExpr() : Expr(FLOAT_LITERAL_EXPR)
    {
    }

public:
    FloatLiteralExpr(double val, std::unique_ptr<IdentifierExpr> pType)
        : Expr(FLOAT_LITERAL_EXPR), type(std::move(pType)), value(val)
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
                CEREAL_NVP(value));
    }

    /// Type of the literal
    std::unique_ptr<IdentifierExpr> type;
    /// Value of the literal
    double value{0.0};
};

/// String literal
class StringLiteralExpr : public Expr
{
    friend class cereal::access;

    StringLiteralExpr() : Expr(STRING_LITERAL_EXPR)
    {
    }

public:
    StringLiteralExpr(std::string&& val, std::unique_ptr<IdentifierExpr> pType)
        : Expr(STRING_LITERAL_EXPR), type(std::move(pType)),
          value(std::move(val))
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
                CEREAL_NVP(value));
    }

    /// Type of the literal
    std::unique_ptr<IdentifierExpr> type;
    /// Value of the literal
    std::string value;
};

/// Character literal
class CharLiteralExpr : public Expr
{
    friend class cereal::access;

    CharLiteralExpr() : Expr(CHAR_LITERAL_EXPR)
    {
    }

public:
    CharLiteralExpr(char32_t val, std::unique_ptr<IdentifierExpr> pType)
        : Expr(CHAR_LITERAL_EXPR), type(std::move(pType)), value(val)
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
                CEREAL_NVP(value));
    }

    /// Type of the literal
    std::unique_ptr<IdentifierExpr> type;
    /// Value of the literal
    char32_t value{U'\0'};
};

/// Boolean literal
class BoolLiteralExpr : public Expr
{
    friend class cereal::access;

    BoolLiteralExpr() : Expr(BOOL_LITERAL_EXPR)
    {
    }

public:
    explicit BoolLiteralExpr(bool val)
        : Expr(BOOL_LITERAL_EXPR),
          type(std::make_unique<IdentifierExpr>("bool")), value(val)
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
                CEREAL_NVP(value));
    }

    /// Type of the literal
    std::unique_ptr<IdentifierExpr> type;
    /// Value of the literal
    bool value{true};
};
} // namespace ast
