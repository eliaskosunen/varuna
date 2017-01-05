/*
Copyright (C) 2016-2017 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/Visitor.h"

namespace core
{
namespace ast
{
    class ASTParentSolverVisitor : public Visitor
    {
    public:
        ASTParentSolverVisitor() = default;

        template <typename T>
        void run(T* root)
        {
            auto castedRoot = dynamic_cast<ASTNode*>(root);
            if(!castedRoot)
            {
                throw std::invalid_argument(
                    "Invalid root node given to ASTParentSolverVisitor");
            }
            root->accept(this, nullptr);
        }

        void visit(ASTNode* node, ASTNode* parent);
        void visit(ASTStatement* node, ASTNode* parent);
        void visit(ASTExpression* node, ASTNode* parent);

        void visit(ASTIfStatement* node, ASTNode* parent);
        void visit(ASTForStatement* node, ASTNode* parent);
        void visit(ASTForeachStatement* node, ASTNode* parent);
        void visit(ASTWhileStatement* node, ASTNode* parent);
        void visit(ASTImportStatement* node, ASTNode* parent);
        void visit(ASTModuleStatement* node, ASTNode* parent);

        void visit(ASTEmptyExpression* node, ASTNode* parent);
        void visit(ASTIdentifierExpression* node, ASTNode* parent);
        void visit(ASTVariableRefExpression* node, ASTNode* parent);
        void visit(ASTCallExpression* node, ASTNode* parent);
        void visit(ASTCastExpression* node, ASTNode* parent);
        void visit(ASTVariableDefinitionExpression* node, ASTNode* parent);
        void visit(ASTSubscriptExpression* node, ASTNode* parent);
        void visit(ASTSubscriptRangedExpression* node, ASTNode* parent);
        void visit(ASTMemberAccessExpression* node, ASTNode* parent);

        void visit(ASTFunctionParameter* node, ASTNode* parent);
        void visit(ASTFunctionPrototypeStatement* node, ASTNode* parent);
        void visit(ASTFunctionDefinitionStatement* node, ASTNode* parent);
        void visit(ASTFunctionDeclarationStatement* node, ASTNode* parent);
        void visit(ASTReturnStatement* node, ASTNode* parent);

        void visit(ASTIntegerLiteralExpression* node, ASTNode* parent);
        void visit(ASTFloatLiteralExpression* node, ASTNode* parent);
        void visit(ASTStringLiteralExpression* node, ASTNode* parent);
        void visit(ASTCharLiteralExpression* node, ASTNode* parent);
        void visit(ASTBoolLiteralExpression* node, ASTNode* parent);
        void visit(ASTNoneLiteralExpression* node, ASTNode* parent);

        void visit(ASTBinaryOperationExpression* node, ASTNode* parent);
        void visit(ASTUnaryOperationExpression* node, ASTNode* parent);
        void visit(ASTAssignmentOperationExpression* node, ASTNode* parent);

        void visit(ASTEmptyStatement* node, ASTNode* parent);
        void visit(ASTBlockStatement* node, ASTNode* parent);
        void visit(ASTWrappedExpressionStatement* node, ASTNode* parent);
    };
} // namespace ast
} // namespace core
