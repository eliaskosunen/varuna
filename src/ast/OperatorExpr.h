// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/Expr.h"
#include "ast/FwdDecl.h"
#include "util/OperatorType.h"
#include <cereal.h>

namespace ast
{
/// Binary-operand expression
class BinaryExpr : public Expr
{
    friend class cereal::access;

    BinaryExpr() : Expr(BINARY_EXPR)
    {
    }

public:
    BinaryExpr(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r,
               util::OperatorType o)
        : Expr(BINARY_EXPR), lhs(std::move(l)), rhs(std::move(r)), oper(o)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Expr>(this), CEREAL_NVP(lhs),
                CEREAL_NVP(rhs), CEREAL_NVP(oper));
    }

    /// Left-hand side
    std::unique_ptr<Expr> lhs;
    /// Right-hand side
    std::unique_ptr<Expr> rhs;
    /// Operator
    util::OperatorType oper{util::OPERATOR_DEFAULT};
};

/// Unary-operand expression
class UnaryExpr : public Expr
{
    friend class cereal::access;

    UnaryExpr() : Expr(UNARY_EXPR)
    {
    }

public:
    UnaryExpr(std::unique_ptr<Expr> _operand, util::OperatorType o)
        : Expr(UNARY_EXPR), operand(std::move(_operand)), oper(o)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Expr>(this), CEREAL_NVP(operand),
                CEREAL_NVP(oper));
    }

    /// Operand
    std::unique_ptr<Expr> operand;
    /// Operator
    util::OperatorType oper{util::OPERATOR_DEFAULT};
};

/// Assignment expression
class AssignmentExpr : public Expr
{
    friend class cereal::access;

    AssignmentExpr() : Expr(ASSIGNMENT_EXPR)
    {
    }

public:
    AssignmentExpr(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r,
                   util::OperatorType o)
        : Expr(ASSIGNMENT_EXPR), lhs(std::move(l)), rhs(std::move(r)), oper(o)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Expr>(this), CEREAL_NVP(lhs),
                CEREAL_NVP(rhs), CEREAL_NVP(oper));
    }

    /// Left-hand side
    std::unique_ptr<Expr> lhs;
    /// Right-hand side
    std::unique_ptr<Expr> rhs;
    /// Operator
    util::OperatorType oper{util::OPERATOR_DEFAULT};
};

/// Arbitrary-operand expression
class ArbitraryOperandExpr : public Expr
{
    friend class cereal::access;

    ArbitraryOperandExpr() : Expr(ARBITRARY_OPERAND_EXPR)
    {
    }

public:
    ArbitraryOperandExpr(std::vector<std::unique_ptr<Expr>> pOperands,
                         util::OperatorType o)
        : Expr(ARBITRARY_OPERAND_EXPR), operands(std::move(pOperands)), oper(o)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Expr>(this), CEREAL_NVP(operands),
                CEREAL_NVP(oper));
    }

    /// Operand list
    std::vector<std::unique_ptr<Expr>> operands;
    /// Operator
    util::OperatorType oper{util::OPERATOR_DEFAULT};
};
} // namespace ast
