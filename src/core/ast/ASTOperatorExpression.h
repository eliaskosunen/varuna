// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTExpression.h"
#include "core/ast/FwdDecl.h"
#include "core/lexer/Token.h"

namespace core
{
namespace ast
{
    class ASTBinaryOperationExpression : public ASTExpression
    {
    public:
        ASTBinaryOperationExpression(std::unique_ptr<ASTExpression> l,
                                     std::unique_ptr<ASTExpression> r,
                                     core::lexer::TokenType o)
            : ASTExpression(BINARY_OPERATION_EXPR), lhs(std::move(l)),
              rhs(std::move(r)), oper(o)
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTExpression> lhs, rhs;
        core::lexer::TokenType oper;
    };

    class ASTUnaryOperationExpression : public ASTExpression
    {
    public:
        ASTUnaryOperationExpression(std::unique_ptr<ASTExpression> _operand,
                                    core::lexer::TokenType o)
            : ASTExpression(UNARY_OPERATION_EXPR), operand(std::move(_operand)),
              oper(o)
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTExpression> operand;
        core::lexer::TokenType oper;
    };

    class ASTAssignmentOperationExpression : public ASTExpression
    {
    public:
        ASTAssignmentOperationExpression(
            std::unique_ptr<ASTIdentifierExpression> l,
            std::unique_ptr<ASTExpression> r, core::lexer::TokenType o)
            : ASTExpression(ASSIGNMENT_OPERATION_EXPR), lhs(std::move(l)),
              rhs(std::move(r)), oper(o)
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTIdentifierExpression> lhs;
        std::unique_ptr<ASTExpression> rhs;
        core::lexer::TokenType oper;
    };
} // namespace ast
} // namespace core
