// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/ParentSolverVisitor.h"
#include "ast/ControlStmt.h"
#include "ast/Expr.h"
#include "ast/FunctionStmt.h"
#include "ast/FwdDecl.h"
#include "ast/LiteralExpr.h"
#include "ast/Node.h"
#include "ast/OperatorExpr.h"
#include "ast/Stmt.h"

namespace ast
{
void ParentSolverVisitor::visit(Node* node, Node* parent)
{
    node->parent = parent;
}
void ParentSolverVisitor::visit(Stmt* node, Node* parent)
{
    node->parent = parent;
}
void ParentSolverVisitor::visit(Expr* node, Node* parent)
{
    node->parent = parent;
}

void ParentSolverVisitor::visit(IfStmt* node, Node* parent)
{
    node->parent = parent;

    node->condition->accept(this, node);
    node->ifBlock->accept(this, node);
    node->elseBlock->accept(this, node);
}
void ParentSolverVisitor::visit(ForStmt* node, Node* parent)
{
    node->parent = parent;

    node->init->accept(this, node);
    node->end->accept(this, node);
    node->step->accept(this, node);
    node->block->accept(this, node);
}
void ParentSolverVisitor::visit(ForeachStmt* node, Node* parent)
{
    node->parent = parent;

    node->iteratee->accept(this, node);
    node->iterator->accept(this, node);
    node->block->accept(this, node);
}
void ParentSolverVisitor::visit(WhileStmt* node, Node* parent)
{
    node->parent = parent;

    node->condition->accept(this, node);
    node->block->accept(this, node);
}
void ParentSolverVisitor::visit(ImportStmt* node, Node* parent)
{
    node->parent = parent;

    node->importee->accept(this, node);
}
void ParentSolverVisitor::visit(ModuleStmt* node, Node* parent)
{
    node->parent = parent;

    node->moduleName->accept(this, node);
}

void ParentSolverVisitor::visit(EmptyExpr* node, Node* parent)
{
    node->parent = parent;
}
void ParentSolverVisitor::visit(IdentifierExpr* node, Node* parent)
{
    node->parent = parent;
}
void ParentSolverVisitor::visit(VariableRefExpr* node, Node* parent)
{
    node->parent = parent;
}
void ParentSolverVisitor::visit(VariableDefinitionExpr* node, Node* parent)
{
    node->parent = parent;

    node->type->accept(this, node);
    node->name->accept(this, node);
    node->init->accept(this, node);
}
void ParentSolverVisitor::visit(GlobalVariableDefinitionExpr* node,
                                Node* parent)
{
    node->parent = parent;

    node->var->accept(this, node);
}

void ParentSolverVisitor::visit(FunctionParameter* node, Node* parent)
{
    node->parent = parent;

    node->var->accept(this, node);
}
void ParentSolverVisitor::visit(FunctionPrototypeStmt* node, Node* parent)
{
    node->parent = parent;

    node->name->accept(this, node);
    node->returnType->accept(this, node);

    auto& params = node->params;
    for(auto& p : params)
    {
        p->accept(this, node);
    }
}
void ParentSolverVisitor::visit(FunctionDefinitionStmt* node, Node* parent)
{
    node->parent = parent;

    node->proto->accept(this, node);
    node->body->accept(this, node);
}
void ParentSolverVisitor::visit(ReturnStmt* node, Node* parent)
{
    node->parent = parent;

    node->returnValue->accept(this, node);
}

void ParentSolverVisitor::visit(IntegerLiteralExpr* node, Node* parent)
{
    node->parent = parent;

    node->type->accept(this, node);
}
void ParentSolverVisitor::visit(FloatLiteralExpr* node, Node* parent)
{
    node->parent = parent;

    node->type->accept(this, node);
}
void ParentSolverVisitor::visit(StringLiteralExpr* node, Node* parent)
{
    node->parent = parent;

    node->type->accept(this, node);
}
void ParentSolverVisitor::visit(CharLiteralExpr* node, Node* parent)
{
    node->parent = parent;

    node->type->accept(this, node);
}
void ParentSolverVisitor::visit(BoolLiteralExpr* node, Node* parent)
{
    node->parent = parent;

    node->type->accept(this, node);
}

void ParentSolverVisitor::visit(BinaryExpr* node, Node* parent)
{
    node->parent = parent;

    node->lhs->accept(this, node);
    node->rhs->accept(this, node);
}
void ParentSolverVisitor::visit(UnaryExpr* node, Node* parent)
{
    node->parent = parent;

    node->operand->accept(this, node);
}
void ParentSolverVisitor::visit(AssignmentExpr* node, Node* parent)
{
    node->parent = parent;

    node->lhs->accept(this, node);
    node->rhs->accept(this, node);
}
void ParentSolverVisitor::visit(ArbitraryOperandExpr* node, Node* parent)
{
    node->parent = parent;

    for(auto& o : node->operands)
    {
        o->accept(this, node);
    }
}

void ParentSolverVisitor::visit(EmptyStmt* node, Node* parent)
{
    node->parent = parent;
}
void ParentSolverVisitor::visit(BlockStmt* node, Node* parent)
{
    node->parent = parent;

    auto& nodes = node->nodes;
    for(auto& n : nodes)
    {
        n->accept(this, node);
    }
}
void ParentSolverVisitor::visit(ExprStmt* node, Node* parent)
{
    node->parent = parent;

    node->expr->accept(this, node);
}
void ParentSolverVisitor::visit(AliasStmt* node, Node* parent)
{
    node->parent = parent;

    node->alias->accept(this, node);
    node->aliasee->accept(this, node);
}

void Expr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void Stmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}

void IfStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void ForStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void ForeachStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void WhileStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void ImportStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void ModuleStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}

void EmptyExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void IdentifierExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void VariableRefExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void VariableDefinitionExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void GlobalVariableDefinitionExpr::accept(ast::ParentSolverVisitor* v,
                                          ast::Node* p)
{
    return v->visit(this, p);
}

void FunctionParameter::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void FunctionPrototypeStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void FunctionDefinitionStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void ReturnStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}

void IntegerLiteralExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void FloatLiteralExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void StringLiteralExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void CharLiteralExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void BoolLiteralExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}

void BinaryExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void UnaryExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void AssignmentExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void ArbitraryOperandExpr::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}

void EmptyStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void BlockStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void ExprStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
void AliasStmt::accept(ast::ParentSolverVisitor* v, ast::Node* p)
{
    return v->visit(this, p);
}
} // namespace ast
