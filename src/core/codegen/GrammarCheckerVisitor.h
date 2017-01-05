// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/Visitor.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace core
{
namespace codegen
{
    class GrammarCheckerVisitor : public ast::Visitor
    {
    public:
        GrammarCheckerVisitor() = default;

        template <typename T>
        bool run(T root)
        {
            auto castedRoot = dynamic_cast<ast::ASTNode*>(root);
            if(!castedRoot)
            {
                throw std::invalid_argument(
                    "Invalid root node given to GrammarCheckerVisitor");
            }
            return root->accept(this);
        }

        template <typename... Args>
        bool grammarError(const std::string& format, Args... args) const
        {
            util::logger->error(format.c_str(), args...);
            return false;
        }

        template <typename... Args>
        void grammarWarning(const std::string& format, Args... args) const
        {
            util::logger->warn(format.c_str(), args...);
        }

        bool visit(ast::ASTNode* node) = delete;
        bool visit(ast::ASTStatement* node);
        bool visit(ast::ASTExpression* node);

        bool visit(ast::ASTIfStatement* node);
        bool visit(ast::ASTForStatement* node);
        bool visit(ast::ASTForeachStatement* node);
        bool visit(ast::ASTWhileStatement* node);
        bool visit(ast::ASTImportStatement* node);
        bool visit(ast::ASTModuleStatement* node);

        bool visit(ast::ASTEmptyExpression* node);
        bool visit(ast::ASTIdentifierExpression* node);
        bool visit(ast::ASTVariableRefExpression* node);
        bool visit(ast::ASTCallExpression* node);
        bool visit(ast::ASTCastExpression* node);
        bool visit(ast::ASTVariableDefinitionExpression* node);
        bool visit(ast::ASTSubscriptExpression* node);
        bool visit(ast::ASTSubscriptRangedExpression* node);
        bool visit(ast::ASTMemberAccessExpression* node);

        bool visit(ast::ASTFunctionParameter* node) = delete;
        bool visit(ast::ASTFunctionPrototypeStatement* node);
        bool visit(ast::ASTFunctionDefinitionStatement* node);
        bool visit(ast::ASTFunctionDeclarationStatement* node);
        bool visit(ast::ASTReturnStatement* node);

        bool visit(ast::ASTIntegerLiteralExpression* node);
        bool visit(ast::ASTFloatLiteralExpression* node);
        bool visit(ast::ASTStringLiteralExpression* node);
        bool visit(ast::ASTCharLiteralExpression* node);
        bool visit(ast::ASTBoolLiteralExpression* node);
        bool visit(ast::ASTNoneLiteralExpression* node);

        bool visit(ast::ASTBinaryOperationExpression* node);
        bool visit(ast::ASTUnaryOperationExpression* node);
        bool visit(ast::ASTAssignmentOperationExpression* node);

        bool visit(ast::ASTEmptyStatement* node);
        bool visit(ast::ASTBlockStatement* node);
        bool visit(ast::ASTWrappedExpressionStatement* node);
    };
}
}
