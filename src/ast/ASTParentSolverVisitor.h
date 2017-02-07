// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTNode.h"
#include "ast/FwdDecl.h"
#include "ast/Visitor.h"

namespace ast
{
/// Solves the parents of ASTNdes
class ASTParentSolverVisitor final : public Visitor
{
public:
    ASTParentSolverVisitor() = default;

    /**
     * Run the visitor
     * @param root Pointer to ASTNode or similar, root node of the tree
     */
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
    void visit(ASTCastExpression* node, ASTNode* parent);
    void visit(ASTVariableDefinitionExpression* node, ASTNode* parent);
    void visit(ASTGlobalVariableDefinitionExpression* node, ASTNode* parent);
    void visit(ASTSubscriptExpression* node, ASTNode* parent);
    void visit(ASTSubscriptRangedExpression* node, ASTNode* parent);
    void visit(ASTMemberAccessExpression* node, ASTNode* parent);

    void visit(ASTFunctionParameter* node, ASTNode* parent);
    void visit(ASTFunctionPrototypeStatement* node, ASTNode* parent);
    void visit(ASTFunctionDefinitionStatement* node, ASTNode* parent);
    void visit(ASTReturnStatement* node, ASTNode* parent);

    void visit(ASTIntegerLiteralExpression* node, ASTNode* parent);
    void visit(ASTFloatLiteralExpression* node, ASTNode* parent);
    void visit(ASTStringLiteralExpression* node, ASTNode* parent);
    void visit(ASTCharLiteralExpression* node, ASTNode* parent);
    void visit(ASTBoolLiteralExpression* node, ASTNode* parent);
    void visit(ASTNoneLiteralExpression* node, ASTNode* parent);

    void visit(ASTBinaryExpression* node, ASTNode* parent);
    void visit(ASTUnaryExpression* node, ASTNode* parent);
    void visit(ASTAssignmentExpression* node, ASTNode* parent);
    void visit(ASTArbitraryOperandExpression* node, ASTNode* parent);

    void visit(ASTEmptyStatement* node, ASTNode* parent);
    void visit(ASTBlockStatement* node, ASTNode* parent);
    void visit(ASTWrappedExpressionStatement* node, ASTNode* parent);
};
} // namespace ast
