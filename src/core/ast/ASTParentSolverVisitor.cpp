// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/ast/ASTParentSolverVisitor.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"

namespace core
{
namespace ast
{
    void ASTParentSolverVisitor::visit(ASTNode* node, ASTNode* parent)
    {
        node->parent = parent;
    }
    void ASTParentSolverVisitor::visit(ASTStatement* node, ASTNode* parent)
    {
        node->parent = parent;
    }
    void ASTParentSolverVisitor::visit(ASTExpression* node, ASTNode* parent)
    {
        node->parent = parent;
    }

    void ASTParentSolverVisitor::visit(ASTIfStatement* node, ASTNode* parent)
    {
        node->parent = parent;

        node->condition->accept(this, node);
        node->ifBlock->accept(this, node);
        node->elseBlock->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTForStatement* node, ASTNode* parent)
    {
        node->parent = parent;

        node->init->accept(this, node);
        node->end->accept(this, node);
        node->step->accept(this, node);
        node->block->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTForeachStatement* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->iteratee->accept(this, node);
        node->iterator->accept(this, node);
        node->block->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTWhileStatement* node, ASTNode* parent)
    {
        node->parent = parent;

        node->condition->accept(this, node);
        node->block->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTImportStatement* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->importee->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTModuleStatement* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->moduleName->accept(this, node);
    }

    void ASTParentSolverVisitor::visit(ASTEmptyExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;
    }
    void ASTParentSolverVisitor::visit(ASTIdentifierExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;
    }
    void ASTParentSolverVisitor::visit(ASTVariableRefExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;
    }
    void ASTParentSolverVisitor::visit(ASTCastExpression* node, ASTNode* parent)
    {
        node->parent = parent;

        node->type->accept(this, node);
        node->castee->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTVariableDefinitionExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->type->accept(this, node);
        node->name->accept(this, node);
        node->init->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTSubscriptExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->lhs->accept(this, node);
        node->index->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTSubscriptRangedExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;
    }
    void ASTParentSolverVisitor::visit(ASTMemberAccessExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->lhs->accept(this, node);
        node->rhs->accept(this, node);
    }

    void ASTParentSolverVisitor::visit(ASTFunctionParameter* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->var->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTFunctionPrototypeStatement* node,
                                       ASTNode* parent)
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
    void ASTParentSolverVisitor::visit(ASTFunctionDefinitionStatement* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->proto->accept(this, node);
        node->body->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTReturnStatement* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->returnValue->accept(this, node);
    }

    void ASTParentSolverVisitor::visit(ASTIntegerLiteralExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->type->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTFloatLiteralExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->type->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTStringLiteralExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->type->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTCharLiteralExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->type->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTBoolLiteralExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->type->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTNoneLiteralExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->type->accept(this, node);
    }

    void ASTParentSolverVisitor::visit(ASTBinaryOperationExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->lhs->accept(this, node);
        node->rhs->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTUnaryOperationExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->operand->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTAssignmentOperationExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->lhs->accept(this, node);
        node->rhs->accept(this, node);
    }
    void ASTParentSolverVisitor::visit(ASTArbitraryOperationExpression* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        for(auto& o : node->operands)
        {
            o->accept(this, node);
        }
    }

    void ASTParentSolverVisitor::visit(ASTEmptyStatement* node, ASTNode* parent)
    {
        node->parent = parent;
    }
    void ASTParentSolverVisitor::visit(ASTBlockStatement* node, ASTNode* parent)
    {
        node->parent = parent;

        auto& nodes = node->nodes;
        for(auto& n : nodes)
        {
            n->accept(this, node);
        }
    }
    void ASTParentSolverVisitor::visit(ASTWrappedExpressionStatement* node,
                                       ASTNode* parent)
    {
        node->parent = parent;

        node->expr->accept(this, node);
    }
} // namespace ast
} // namespace core

void core::ast::ASTExpression::accept(core::ast::ASTParentSolverVisitor* v,
                                      core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                     core::ast::ASTNode* p)
{
    return v->visit(this, p);
}

void core::ast::ASTIfStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                       core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTForStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                        core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTForeachStatement::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTWhileStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                          core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTImportStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                           core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTModuleStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                           core::ast::ASTNode* p)
{
    return v->visit(this, p);
}

void core::ast::ASTEmptyExpression::accept(core::ast::ASTParentSolverVisitor* v,
                                           core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTIdentifierExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTVariableRefExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTCastExpression::accept(core::ast::ASTParentSolverVisitor* v,
                                          core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTVariableDefinitionExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTSubscriptExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTSubscriptRangedExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTMemberAccessExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}

void core::ast::ASTFunctionParameter::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTFunctionPrototypeStatement::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTFunctionDefinitionStatement::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTReturnStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                           core::ast::ASTNode* p)
{
    return v->visit(this, p);
}

void core::ast::ASTIntegerLiteralExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTFloatLiteralExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTStringLiteralExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTCharLiteralExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTBoolLiteralExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTNoneLiteralExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}

void core::ast::ASTBinaryOperationExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTUnaryOperationExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTAssignmentOperationExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTArbitraryOperationExpression::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}

void core::ast::ASTEmptyStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                          core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTBlockStatement::accept(core::ast::ASTParentSolverVisitor* v,
                                          core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
void core::ast::ASTWrappedExpressionStatement::accept(
    core::ast::ASTParentSolverVisitor* v, core::ast::ASTNode* p)
{
    return v->visit(this, p);
}
