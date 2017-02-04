// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"

namespace ast
{
class ASTFunctionParameter : public ASTStatement
{
public:
    ASTFunctionParameter(std::unique_ptr<ASTVariableDefinitionExpression> pVar,
                         uint32_t pNum)
        : ASTStatement(FUNCTION_PARAMETER), var(std::move(pVar)), num(pNum)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;

    std::unique_ptr<ASTVariableDefinitionExpression> var;
    uint32_t num;
};

class ASTFunctionPrototypeStatement : public ASTStatement
{
public:
    ASTFunctionPrototypeStatement(
        std::unique_ptr<ASTIdentifierExpression> pName,
        std::unique_ptr<ASTIdentifierExpression> retType,
        std::vector<std::unique_ptr<ASTFunctionParameter>> pParams)
        : ASTStatement(FUNCTION_PROTO_STMT), name(std::move(pName)),
          returnType(std::move(retType)), params(std::move(pParams))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTIdentifierExpression> name, returnType;
    std::vector<std::unique_ptr<ASTFunctionParameter>> params;
    bool isMain{false};
};

class ASTFunctionDefinitionStatement : public ASTStatement
{
public:
    ASTFunctionDefinitionStatement(
        std::unique_ptr<ASTFunctionPrototypeStatement> pProto,
        std::unique_ptr<ASTBlockStatement> pBody)
        : ASTStatement(FUNCTION_DEF_STMT), proto(std::move(pProto)),
          body(std::move(pBody))
    {
    }

    ASTFunctionDefinitionStatement(
        std::unique_ptr<ASTFunctionPrototypeStatement> pProto,
        std::unique_ptr<ASTEmptyStatement> pBody)
        : ASTStatement(FUNCTION_DEF_STMT), proto(std::move(pProto)),
          body(std::make_unique<ASTBlockStatement>()), isDecl(true)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTFunctionPrototypeStatement> proto;
    std::unique_ptr<ASTBlockStatement> body;
    bool isDecl{false};
};

class ASTReturnStatement : public ASTStatement
{
public:
    explicit ASTReturnStatement(std::unique_ptr<ASTExpression> retval)
        : ASTStatement(RETURN_STMT), returnValue(std::move(retval))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTExpression> returnValue;
};
} // namespace ast
