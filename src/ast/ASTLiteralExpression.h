// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTExpression.h"
#include "ast/ASTOperatorExpression.h"
#include "ast/FwdDecl.h"
#include "util/SafeEnum.h"

namespace ast
{
class ASTIntegerLiteralExpression : public ASTExpression
{
public:
    ASTIntegerLiteralExpression(int64_t val,
                                std::unique_ptr<ASTIdentifierExpression> pType,
                                bool pIsSigned = true)
        : ASTExpression(INTEGER_LITERAL_EXPR), value(val), isSigned(pIsSigned),
          type(std::move(pType))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    int64_t value;
    bool isSigned;
    std::unique_ptr<ASTIdentifierExpression> type;
};

class ASTFloatLiteralExpression : public ASTExpression
{
public:
    ASTFloatLiteralExpression(double val,
                              std::unique_ptr<ASTIdentifierExpression> pType)
        : ASTExpression(FLOAT_LITERAL_EXPR), value(val), type(std::move(pType))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    double value;
    std::unique_ptr<ASTIdentifierExpression> type;
};

class ASTStringLiteralExpression : public ASTExpression
{
public:
    explicit ASTStringLiteralExpression(const std::string& val)
        : ASTExpression(STRING_LITERAL_EXPR), value(val),
          type(std::make_unique<ASTIdentifierExpression>("string"))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    const std::string& value;
    std::unique_ptr<ASTIdentifierExpression> type;
};

class ASTCharLiteralExpression : public ASTExpression
{
public:
    ASTCharLiteralExpression(char32_t val,
                             std::unique_ptr<ASTIdentifierExpression> pType)
        : ASTExpression(CHAR_LITERAL_EXPR), value(val), type(std::move(pType))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    char32_t value;
    std::unique_ptr<ASTIdentifierExpression> type;
};

class ASTBoolLiteralExpression : public ASTExpression
{
public:
    explicit ASTBoolLiteralExpression(bool val)
        : ASTExpression(BOOL_LITERAL_EXPR), value(val),
          type(std::make_unique<ASTIdentifierExpression>("bool"))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    bool value;
    std::unique_ptr<ASTIdentifierExpression> type;
};

class ASTNoneLiteralExpression : public ASTExpression
{
public:
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

    std::unique_ptr<ASTIdentifierExpression> type;
};
} // namespace ast
