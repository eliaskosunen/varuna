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
/// Integer literal
class ASTIntegerLiteralExpression : public ASTExpression
{
public:
    ASTIntegerLiteralExpression(int64_t val,
                                std::unique_ptr<ASTIdentifierExpression> pType,
                                bool pIsSigned = true)
        : ASTExpression(INTEGER_LITERAL_EXPR), type(std::move(pType)),
          value(val), isSigned(pIsSigned)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Type of the literal
    std::unique_ptr<ASTIdentifierExpression> type;
    /// Value of the literal
    int64_t value;
    /// Is a signed integer
    bool isSigned;
};

/// Floating-point literal
class ASTFloatLiteralExpression : public ASTExpression
{
public:
    ASTFloatLiteralExpression(double val,
                              std::unique_ptr<ASTIdentifierExpression> pType)
        : ASTExpression(FLOAT_LITERAL_EXPR), type(std::move(pType)), value(val)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Type of the literal
    std::unique_ptr<ASTIdentifierExpression> type;
    /// Value of the literal
    double value;
};

/// String literal
class ASTStringLiteralExpression : public ASTExpression
{
public:
    ASTStringLiteralExpression(std::string&& val,
                               std::unique_ptr<ASTIdentifierExpression> pType)
        : ASTExpression(STRING_LITERAL_EXPR), type(std::move(pType)),
          value(std::move(val))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Type of the literal
    std::unique_ptr<ASTIdentifierExpression> type;
    /// Value of the literal
    std::string value;
};

/// Character literal
class ASTCharLiteralExpression : public ASTExpression
{
public:
    ASTCharLiteralExpression(char32_t val,
                             std::unique_ptr<ASTIdentifierExpression> pType)
        : ASTExpression(CHAR_LITERAL_EXPR), type(std::move(pType)), value(val)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Type of the literal
    std::unique_ptr<ASTIdentifierExpression> type;
    /// Value of the literal
    char32_t value;
};

/// Boolean literal
class ASTBoolLiteralExpression : public ASTExpression
{
public:
    explicit ASTBoolLiteralExpression(bool val)
        : ASTExpression(BOOL_LITERAL_EXPR),
          type(std::make_unique<ASTIdentifierExpression>("bool")), value(val)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Type of the literal
    std::unique_ptr<ASTIdentifierExpression> type;
    /// Value of the literal
    bool value;
};

/// None literal
/// Unused
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
