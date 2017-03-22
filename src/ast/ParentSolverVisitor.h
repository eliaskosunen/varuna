// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/FwdDecl.h"
#include "ast/Node.h"
#include "ast/Visitor.h"

namespace ast
{
/// Solves the parents of ASTNdes
class ParentSolverVisitor final : public Visitor
{
public:
    ParentSolverVisitor() = default;

    /**
     * Run the visitor
     * @param root Pointer to Node or similar, root node of the tree
     */
    template <typename T>
    void run(T* root)
    {
        root->accept(this, nullptr);
    }

    void visit(Node* node, Node* parent);
    void visit(Stmt* node, Node* parent);
    void visit(Expr* node, Node* parent);

    void visit(IfStmt* node, Node* parent);
    void visit(ForStmt* node, Node* parent);
    void visit(ForeachStmt* node, Node* parent);
    void visit(WhileStmt* node, Node* parent);
    void visit(ImportStmt* node, Node* parent);
    void visit(ModuleStmt* node, Node* parent);

    void visit(EmptyExpr* node, Node* parent);
    void visit(IdentifierExpr* node, Node* parent);
    void visit(VariableRefExpr* node, Node* parent);
    void visit(VariableDefinitionExpr* node, Node* parent);
    void visit(GlobalVariableDefinitionExpr* node, Node* parent);

    void visit(FunctionParameter* node, Node* parent);
    void visit(FunctionPrototypeStmt* node, Node* parent);
    void visit(FunctionDefinitionStmt* node, Node* parent);
    void visit(ReturnStmt* node, Node* parent);

    void visit(IntegerLiteralExpr* node, Node* parent);
    void visit(FloatLiteralExpr* node, Node* parent);
    void visit(StringLiteralExpr* node, Node* parent);
    void visit(CharLiteralExpr* node, Node* parent);
    void visit(BoolLiteralExpr* node, Node* parent);

    void visit(BinaryExpr* node, Node* parent);
    void visit(UnaryExpr* node, Node* parent);
    void visit(AssignmentExpr* node, Node* parent);
    void visit(ArbitraryOperandExpr* node, Node* parent);

    void visit(EmptyStmt* node, Node* parent);
    void visit(BlockStmt* node, Node* parent);
    void visit(ExprStmt* node, Node* parent);
    void visit(AliasStmt* node, Node* parent);
};
} // namespace ast
