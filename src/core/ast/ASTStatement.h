// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTExpression.h"
#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include <memory>
#include <vector>

namespace core
{
namespace ast
{
    class ASTStatement : public ASTNode
    {
    protected:
        ASTStatement(NodeType t) : ASTNode(t)
        {
        }

    public:
        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        virtual std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v);
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        ASTStatement() : ASTNode(STMT)
        {
        }
        ASTStatement(const ASTStatement&) = delete;
        ASTStatement& operator=(const ASTStatement&) = delete;
        ASTStatement(ASTStatement&&) = default;
        ASTStatement& operator=(ASTStatement&&) = default;
        ~ASTStatement() override = default;
    };

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

    class ASTBlockStatement : public ASTStatement
    {
    public:
        using Statement = std::unique_ptr<ASTStatement>;
        using StatementVector = std::vector<Statement>;
        StatementVector nodes;

        ASTBlockStatement() : ASTStatement(BLOCK_STMT)
        {
        }
        ASTBlockStatement(Statement first) : ASTStatement(BLOCK_STMT)
        {
            nodes.push_back(std::move(first));
        }
        ASTBlockStatement(StatementVector vec)
            : ASTStatement(BLOCK_STMT), nodes(std::move(vec))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTWrappedExpressionStatement : public ASTStatement
    {
    public:
        std::unique_ptr<ASTExpression> expr;

        ASTWrappedExpressionStatement(std::unique_ptr<ASTExpression> expression)
            : ASTStatement(WRAPPED_EXPR_STMT), expr(std::move(expression))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };
} // namespace ast
} // namespace core
