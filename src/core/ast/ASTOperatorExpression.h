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
        std::unique_ptr<ASTExpression> left, right;
        core::lexer::TokenType oper;

        ASTBinaryOperationExpression(std::unique_ptr<ASTExpression> l,
                                     std::unique_ptr<ASTExpression> r,
                                     core::lexer::TokenType o)
            : ASTExpression(BINARY_OPERATION_EXPR), left(std::move(l)),
              right(std::move(r)), oper(o)
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTUnaryOperationExpression : public ASTExpression
    {
    public:
        std::unique_ptr<ASTExpression> operand;
        core::lexer::TokenType oper;

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
    };

    class ASTAssignmentOperationExpression : public ASTExpression
    {
    public:
        std::unique_ptr<ASTIdentifierExpression> lval;
        std::unique_ptr<ASTExpression> rval;
        core::lexer::TokenType oper;

        ASTAssignmentOperationExpression(
            std::unique_ptr<ASTIdentifierExpression> l,
            std::unique_ptr<ASTExpression> r, core::lexer::TokenType o)
            : ASTExpression(ASSIGNMENT_OPERATION_EXPR), lval(std::move(l)),
              rval(std::move(r)), oper(o)
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
