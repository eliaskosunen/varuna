// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"

namespace ast
{
/// Function parameter
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

    /// Parameter
    std::unique_ptr<ASTVariableDefinitionExpression> var;
    /// 1 = first parameter, 2 = second parameter...
    uint32_t num;
};

/// Function prototype
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

    /// Function name
    std::unique_ptr<ASTIdentifierExpression> name;
    /// Function return type
    std::unique_ptr<ASTIdentifierExpression> returnType;
    /// Function parameters
    std::vector<std::unique_ptr<ASTFunctionParameter>> params;
    /// Is function a main function
    bool isMain{false};
};

/// Function definition
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

    /// Function prototype
    std::unique_ptr<ASTFunctionPrototypeStatement> proto;
    /// Function body
    /// Empty block statement if declaration
    std::unique_ptr<ASTBlockStatement> body;
    /// Is a function declaration
    bool isDecl{false};
};

/// return-statement
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

    /// Return value
    std::unique_ptr<ASTExpression> returnValue;
};
} // namespace ast
