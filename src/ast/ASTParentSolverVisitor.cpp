// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/ASTParentSolverVisitor.h"
#include "ast/ASTControlStatement.h"
#include "ast/ASTExpression.h"
#include "ast/ASTFunctionStatement.h"
#include "ast/ASTLiteralExpression.h"
#include "ast/ASTNode.h"
#include "ast/ASTOperatorExpression.h"
#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"

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
void ASTParentSolverVisitor::visit(ASTForeachStatement* node, ASTNode* parent)
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
void ASTParentSolverVisitor::visit(ASTImportStatement* node, ASTNode* parent)
{
    node->parent = parent;

    node->importee->accept(this, node);
}
void ASTParentSolverVisitor::visit(ASTModuleStatement* node, ASTNode* parent)
{
    node->parent = parent;

    node->moduleName->accept(this, node);
}

void ASTParentSolverVisitor::visit(ASTEmptyExpression* node, ASTNode* parent)
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
void ASTParentSolverVisitor::visit(ASTGlobalVariableDefinitionExpression* node,
                                   ASTNode* parent)
{
    node->parent = parent;

    node->var->accept(this, node);
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

void ASTParentSolverVisitor::visit(ASTFunctionParameter* node, ASTNode* parent)
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
void ASTParentSolverVisitor::visit(ASTReturnStatement* node, ASTNode* parent)
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

void ASTParentSolverVisitor::visit(ASTBinaryExpression* node, ASTNode* parent)
{
    node->parent = parent;

    node->lhs->accept(this, node);
    node->rhs->accept(this, node);
}
void ASTParentSolverVisitor::visit(ASTUnaryExpression* node, ASTNode* parent)
{
    node->parent = parent;

    node->operand->accept(this, node);
}
void ASTParentSolverVisitor::visit(ASTAssignmentExpression* node,
                                   ASTNode* parent)
{
    node->parent = parent;

    node->lhs->accept(this, node);
    node->rhs->accept(this, node);
}
void ASTParentSolverVisitor::visit(ASTArbitraryOperandExpression* node,
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

void ast::ASTExpression::accept(ast::ASTParentSolverVisitor* v, ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTStatement::accept(ast::ASTParentSolverVisitor* v, ast::ASTNode* p)
{
    return v->visit(this, p);
}

void ast::ASTIfStatement::accept(ast::ASTParentSolverVisitor* v,
                                 ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTForStatement::accept(ast::ASTParentSolverVisitor* v,
                                  ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTForeachStatement::accept(ast::ASTParentSolverVisitor* v,
                                      ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTWhileStatement::accept(ast::ASTParentSolverVisitor* v,
                                    ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTImportStatement::accept(ast::ASTParentSolverVisitor* v,
                                     ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTModuleStatement::accept(ast::ASTParentSolverVisitor* v,
                                     ast::ASTNode* p)
{
    return v->visit(this, p);
}

void ast::ASTEmptyExpression::accept(ast::ASTParentSolverVisitor* v,
                                     ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTIdentifierExpression::accept(ast::ASTParentSolverVisitor* v,
                                          ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTVariableRefExpression::accept(ast::ASTParentSolverVisitor* v,
                                           ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTCastExpression::accept(ast::ASTParentSolverVisitor* v,
                                    ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTVariableDefinitionExpression::accept(
    ast::ASTParentSolverVisitor* v, ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTGlobalVariableDefinitionExpression::accept(
    ast::ASTParentSolverVisitor* v, ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTSubscriptExpression::accept(ast::ASTParentSolverVisitor* v,
                                         ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTSubscriptRangedExpression::accept(ast::ASTParentSolverVisitor* v,
                                               ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTMemberAccessExpression::accept(ast::ASTParentSolverVisitor* v,
                                            ast::ASTNode* p)
{
    return v->visit(this, p);
}

void ast::ASTFunctionParameter::accept(ast::ASTParentSolverVisitor* v,
                                       ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTFunctionPrototypeStatement::accept(ast::ASTParentSolverVisitor* v,
                                                ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTFunctionDefinitionStatement::accept(ast::ASTParentSolverVisitor* v,
                                                 ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTReturnStatement::accept(ast::ASTParentSolverVisitor* v,
                                     ast::ASTNode* p)
{
    return v->visit(this, p);
}

void ast::ASTIntegerLiteralExpression::accept(ast::ASTParentSolverVisitor* v,
                                              ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTFloatLiteralExpression::accept(ast::ASTParentSolverVisitor* v,
                                            ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTStringLiteralExpression::accept(ast::ASTParentSolverVisitor* v,
                                             ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTCharLiteralExpression::accept(ast::ASTParentSolverVisitor* v,
                                           ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTBoolLiteralExpression::accept(ast::ASTParentSolverVisitor* v,
                                           ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTNoneLiteralExpression::accept(ast::ASTParentSolverVisitor* v,
                                           ast::ASTNode* p)
{
    return v->visit(this, p);
}

void ast::ASTBinaryExpression::accept(ast::ASTParentSolverVisitor* v,
                                      ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTUnaryExpression::accept(ast::ASTParentSolverVisitor* v,
                                     ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTAssignmentExpression::accept(ast::ASTParentSolverVisitor* v,
                                          ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTArbitraryOperandExpression::accept(ast::ASTParentSolverVisitor* v,
                                                ast::ASTNode* p)
{
    return v->visit(this, p);
}

void ast::ASTEmptyStatement::accept(ast::ASTParentSolverVisitor* v,
                                    ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTBlockStatement::accept(ast::ASTParentSolverVisitor* v,
                                    ast::ASTNode* p)
{
    return v->visit(this, p);
}
void ast::ASTWrappedExpressionStatement::accept(ast::ASTParentSolverVisitor* v,
                                                ast::ASTNode* p)
{
    return v->visit(this, p);
}
