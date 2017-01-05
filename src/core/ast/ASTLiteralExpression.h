// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTExpression.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/FwdDecl.h"
#include "core/lexer/Token.h"
#include "util/SafeEnum.h"

namespace core
{
namespace ast
{
    class ASTIntegerLiteralExpression : public ASTExpression
    {
    public:
        int64_t value;
        std::unique_ptr<ASTIdentifierExpression> type;

        ASTIntegerLiteralExpression(
            int64_t val, std::unique_ptr<ASTIdentifierExpression> _type)
            : ASTExpression(INTEGER_LITERAL_EXPR), value(val),
              type(std::move(_type))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTFloatLiteralExpression : public ASTExpression
    {
    public:
        double value;
        std::unique_ptr<ASTIdentifierExpression> type;

        ASTFloatLiteralExpression(
            double val, std::unique_ptr<ASTIdentifierExpression> _type)
            : ASTExpression(FLOAT_LITERAL_EXPR), value(val),
              type(std::move(_type))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTStringLiteralExpression : public ASTExpression
    {
    public:
        const std::string& value;
        std::unique_ptr<ASTIdentifierExpression> type;

        ASTStringLiteralExpression(const std::string& val)
            : ASTExpression(STRING_LITERAL_EXPR), value(val),
              type(std::make_unique<ASTIdentifierExpression>("string"))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTCharLiteralExpression : public ASTExpression
    {
    public:
        char32_t value;
        std::unique_ptr<ASTIdentifierExpression> type;

        ASTCharLiteralExpression(char32_t val)
            : ASTExpression(CHAR_LITERAL_EXPR), value(val),
              type(std::make_unique<ASTIdentifierExpression>("char"))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTBoolLiteralExpression : public ASTExpression
    {
    public:
        bool value;
        std::unique_ptr<ASTIdentifierExpression> type;

        ASTBoolLiteralExpression(bool val)
            : ASTExpression(BOOL_LITERAL_EXPR), value(val),
              type(std::make_unique<ASTIdentifierExpression>("bool"))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTNoneLiteralExpression : public ASTExpression
    {
    public:
        std::unique_ptr<ASTIdentifierExpression> type;

        ASTNoneLiteralExpression()
            : ASTExpression(NONE_LITERAL_EXPR),
              type(std::make_unique<ASTIdentifierExpression>("none_t"))
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
