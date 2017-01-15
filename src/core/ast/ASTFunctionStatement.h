// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"

namespace core
{
namespace ast
{
    class ASTFunctionParameter : public ASTStatement
    {
    public:
        explicit ASTFunctionParameter(
            std::unique_ptr<ASTVariableDefinitionExpression> pVar)
            : ASTStatement(FUNCTION_PARAMETER), var(std::move(pVar))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;

        std::unique_ptr<ASTVariableDefinitionExpression> var;
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

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTFunctionPrototypeStatement> proto;
        std::unique_ptr<ASTBlockStatement> body;
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
} // namespace core
