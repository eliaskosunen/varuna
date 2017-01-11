// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include <memory>
#include <vector>

namespace core
{
namespace ast
{
    class ASTExpression : public ASTNode
    {
    public:
        ASTExpression() : ASTNode(EXPR)
        {
        }
        ASTExpression(const ASTExpression&) = delete;
        ASTExpression& operator=(const ASTExpression&) = delete;
        ASTExpression(ASTExpression&&) noexcept = default;
        ASTExpression& operator=(ASTExpression&&) noexcept = default;
        ~ASTExpression() override = default;

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        virtual std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v);
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

    protected:
        explicit ASTExpression(NodeType t) : ASTNode(t)
        {
        }
    };

    class ASTEmptyExpression : public ASTExpression
    {
    public:
        ASTEmptyExpression() : ASTExpression(EMPTY_EXPR)
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTIdentifierExpression : public ASTExpression
    {
    public:
        explicit ASTIdentifierExpression(std::string val)
            : ASTExpression(IDENTIFIER_EXPR), value(std::move(val))
        {
        }
        ASTIdentifierExpression(const ASTIdentifierExpression&) = delete;
        ASTIdentifierExpression&
        operator=(const ASTIdentifierExpression&) = delete;
        ASTIdentifierExpression(ASTIdentifierExpression&&) noexcept = default;
        ASTIdentifierExpression&
        operator=(ASTIdentifierExpression&&) noexcept = default;
        ~ASTIdentifierExpression() override = default;

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::string value;

    protected:
        ASTIdentifierExpression(NodeType t, std::string val)
            : ASTExpression(t), value(std::move(val))
        {
        }
    };

    class ASTVariableRefExpression : public ASTIdentifierExpression
    {
    public:
        explicit ASTVariableRefExpression(std::string val)
            : ASTIdentifierExpression(VARIABLE_REF_EXPR, val)
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<core::codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTCallExpression : public ASTExpression
    {
    public:
        ASTCallExpression(std::unique_ptr<ASTExpression> pCallee,
                          std::vector<std::unique_ptr<ASTExpression>> pParams)
            : ASTExpression(CALL_EXPR), callee(std::move(pCallee)),
              params(std::move(pParams))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTExpression> callee;
        std::vector<std::unique_ptr<ASTExpression>> params;
    };

    class ASTCastExpression : public ASTExpression
    {
    public:
        ASTCastExpression(std::unique_ptr<ASTExpression> pCastee,
                          std::unique_ptr<ASTIdentifierExpression> pType)
            : ASTExpression(CAST_EXPR), type(std::move(pType)),
              castee(std::move(pCastee))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTIdentifierExpression> type;
        std::unique_ptr<ASTExpression> castee;
    };

    class ASTVariableDefinitionExpression : public ASTExpression
    {
    public:
        ASTVariableDefinitionExpression(
            std::unique_ptr<ASTIdentifierExpression> pType,
            std::unique_ptr<ASTIdentifierExpression> pName,
            std::unique_ptr<ASTExpression> pInit, uint32_t arrSize = 0)
            : ASTExpression(VARIABLE_DEFINITION_EXPR), type(std::move(pType)),
              name(std::move(pName)), init(std::move(pInit)),
              arraySize(arrSize), typeDeduced{true}
        {
        }
        ASTVariableDefinitionExpression(
            std::unique_ptr<ASTIdentifierExpression> pName,
            std::unique_ptr<ASTExpression> pInit)
            : ASTExpression(VARIABLE_DEFINITION_EXPR),
              type(std::make_unique<ASTIdentifierExpression>("")),
              name(std::move(pName)), init(std::move(pInit)), arraySize(0),
              typeDeduced{false}
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTIdentifierExpression> type, name;
        std::unique_ptr<ASTExpression> init;
        uint32_t arraySize;
        bool typeDeduced;
    };

    class ASTSubscriptExpression : public ASTExpression
    {
    protected:
        ASTSubscriptExpression(NodeType t, std::unique_ptr<ASTExpression> lhs_,
                               std::unique_ptr<ASTExpression> index_)
            : ASTExpression(t), lhs(std::move(lhs_)), index(std::move(index_))
        {
        }

    public:
        ASTSubscriptExpression(std::unique_ptr<ASTExpression> pLhs,
                               std::unique_ptr<ASTExpression> pIndex)
            : ASTExpression(SUBSCRIPT_EXPR), lhs(std::move(pLhs)),
              index(std::move(pIndex))
        {
        }

        ASTSubscriptExpression(const ASTSubscriptExpression&) = delete;
        ASTSubscriptExpression(ASTSubscriptExpression&&) noexcept = default;
        ASTSubscriptExpression&
        operator=(const ASTSubscriptExpression&) = delete;
        ASTSubscriptExpression&
        operator=(ASTSubscriptExpression&&) noexcept = default;
        ~ASTSubscriptExpression() override = default;

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTExpression> lhs;
        std::unique_ptr<ASTExpression> index;
    };

    class ASTSubscriptRangedExpression : public ASTSubscriptExpression
    {
    public:
        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTMemberAccessExpression : public ASTExpression
    {
    public:
        ASTMemberAccessExpression(std::unique_ptr<ASTExpression> l,
                                  std::unique_ptr<ASTExpression> r)
            : ASTExpression(MEMBER_ACCESS_EXPR), lhs(std::move(l)),
              rhs(std::move(r))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;

        std::unique_ptr<ASTExpression> lhs, rhs;
    };
} // namespace ast
} // namespace core
