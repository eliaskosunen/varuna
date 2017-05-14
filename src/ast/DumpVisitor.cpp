// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/DumpVisitor.h"
#include "ast/AST.h"
#include "ast/ControlStmt.h"
#include "ast/Expr.h"
#include "ast/FunctionStmt.h"
#include "ast/FwdDecl.h"
#include "ast/LiteralExpr.h"
#include "ast/Node.h"
#include "ast/OperatorExpr.h"
#include "ast/Stmt.h"
#include <string>

namespace ast
{
void Stmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}

void Expr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}

void IfStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void ForStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void ForeachStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void WhileStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void ImportStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void ModuleStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}

void EmptyExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void IdentifierExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void VariableRefExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void VariableDefinitionExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void GlobalVariableDefinitionExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}

void FunctionParameter::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void FunctionPrototypeStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void FunctionDefinitionStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void ReturnStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}

void IntegerLiteralExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void FloatLiteralExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void StringLiteralExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void CharLiteralExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void BoolLiteralExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}

void BinaryExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void UnaryExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void AssignmentExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void ArbitraryOperandExpr::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}

void EmptyStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void BlockStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void ExprStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}
void AliasStmt::accept(DumpVisitor* v, size_t ind)
{
    v->visit(this, ind);
}

void DumpVisitor::visit(Stmt* /*unused*/, size_t ind)
{
    log(ind, "Stmt");
}

void DumpVisitor::visit(Expr* /*unused*/, size_t ind)
{
    log(ind, "Expr");
}

void DumpVisitor::visit(IfStmt* node, size_t ind)
{
    log(ind, "IfStmt:");
    log(ind + 1, "Condition:");
    node->condition->accept(this, ind + 2);
    log(ind + 1, "IfBlock:");
    node->ifBlock->accept(this, ind + 2);
    log(ind + 1, "ElseBlock:");
    node->elseBlock->accept(this, ind + 2);
}
void DumpVisitor::visit(ForStmt* node, size_t ind)
{
    log(ind, "ForStmt:");

    log(ind + 1, "InitExpression:");
    node->init->accept(this, ind + 2);

    log(ind + 1, "EndExpression:");
    node->end->accept(this, ind + 2);

    log(ind + 1, "StepExpression:");
    node->step->accept(this, ind + 2);

    log(ind + 1, "Block:");
    node->block->accept(this, ind + 2);
}
void DumpVisitor::visit(ForeachStmt* node, size_t ind)
{
    log(ind, "ForeachStmt:");
    log(ind + 1, "Iteratee:");
    node->iteratee->accept(this, ind + 2);
    log(ind + 1, "Iterator:");
    node->iterator->accept(this, ind + 2);
    log(ind + 1, "Block:");
    node->block->accept(this, ind + 2);
}
void DumpVisitor::visit(WhileStmt* node, size_t ind)
{
    log(ind, "WhileStmt:");
    log(ind + 1, "Condition:");
    node->condition->accept(this, ind + 2);
    log(ind + 1, "Block:");
    node->block->accept(this, ind + 2);
}
void DumpVisitor::visit(ImportStmt* node, size_t ind)
{
    log(ind, "ImportStmt:");
    node->importee->accept(this, ind + 1);
    log(ind + 1, "isPath: {}", node->isPath);
    log(ind + 1, "importType: {}", node->importType);
}
void DumpVisitor::visit(ModuleStmt* node, size_t ind)
{
    log(ind, "ModuleStmt:");
    node->moduleName->accept(this, ind + 1);
}

void DumpVisitor::visit(EmptyExpr* /*unused*/, size_t ind)
{
    log(ind, "EmptyExpr");
}
void DumpVisitor::visit(IdentifierExpr* node, size_t ind)
{
    log(ind, "IdentifierExpr: {}", node->value);
}
void DumpVisitor::visit(VariableRefExpr* node, size_t ind)
{
    log(ind, "VariableRefExpr: {}", node->value);
}
void DumpVisitor::visit(VariableDefinitionExpr* node, size_t ind)
{
    log(ind, "VariableDefinitionExpr:");
    log(ind + 1, "InferredType: {}", node->typeInferred);
    if(node->typeInferred)
    {
        log(ind + 1, "Type: (will be inferred)");
    }
    else
    {
        log(ind + 1, "Type:");
        node->type->accept(this, ind + 2);
    }
    log(ind + 1, "Name:");
    node->name->accept(this, ind + 2);
    log(ind + 1, "Mutable: {}", node->isMutable);
    log(ind + 1, "InitExpression:");
    node->init->accept(this, ind + 2);
}
void DumpVisitor::visit(GlobalVariableDefinitionExpr* node, size_t ind)
{
    log(ind, "GlobalVariableDefinitionExpr:");
    node->var->accept(this, ind + 1);
}

void DumpVisitor::visit(FunctionParameter* node, size_t ind)
{
    log(ind, "FunctionParameter:");
    node->var->accept(this, ind + 1);
}
void DumpVisitor::visit(FunctionPrototypeStmt* node, size_t ind)
{
    log(ind, "FunctionPrototypeStmt:");
    log(ind + 1, "FunctionName:");
    node->name->accept(this, ind + 2);
    log(ind + 1, "FunctionReturnType:");
    node->returnType->accept(this, ind + 2);
    log(ind + 1, "FunctionParameterList:");
    auto& params = node->params;
    for(auto&& p : params)
    {
        p->accept(this, ind + 2);
    }
}
void DumpVisitor::visit(FunctionDefinitionStmt* node, size_t ind)
{
    log(ind, "FunctionDefinitionStmt:");
    log(ind + 1, "FunctionPrototype:");
    node->proto->accept(this, ind + 2);
    log(ind + 1, "FunctionBody:");
    node->body->accept(this, ind + 2);
}
void DumpVisitor::visit(ReturnStmt* node, size_t ind)
{
    log(ind, "ReturnStmt:");
    node->returnValue->accept(this, ind + 1);
}

void DumpVisitor::visit(IntegerLiteralExpr* node, size_t ind)
{
    log(ind, "IntegerLiteralExpr:");
    log(ind + 1, "Value: {}", node->value);
    log(ind + 1, "Type: {}", node->type->value);
    log(ind + 1, "Signed: {}", node->isSigned);
}
void DumpVisitor::visit(FloatLiteralExpr* node, size_t ind)
{
    log(ind, "FloatLiteralExpr:");
    log(ind + 1, "Value: {}", node->value);
    log(ind + 1, "Type: {}", node->type->value);
}
void DumpVisitor::visit(StringLiteralExpr* node, size_t ind)
{
    log(ind, "StringLiteralExpr:");
    log(ind + 1, "Value: {}", node->value);
    log(ind + 1, "Type: {}", node->type->value);
}
void DumpVisitor::visit(CharLiteralExpr* node, size_t ind)
{
    log(ind, "CharLiteralExpr:");
    log(ind + 1, "Value: {}", node->value);
    log(ind + 1, "Type: {}", node->type->value);
}
void DumpVisitor::visit(BoolLiteralExpr* node, size_t ind)
{
    log(ind, "BoolLiteralExpr:");
    log(ind + 1, "Value: {}", node->value);
    log(ind + 1, "Type: {}", node->type->value);
}

void DumpVisitor::visit(BinaryExpr* node, size_t ind)
{
    log(ind, "BinaryExpr:");
    log(ind + 1, "Operator: {}", node->oper.get());
    log(ind + 1, "LHS:");
    node->lhs->accept(this, ind + 2);
    log(ind + 1, "RHS:");
    node->rhs->accept(this, ind + 2);
}
void DumpVisitor::visit(UnaryExpr* node, size_t ind)
{
    log(ind, "UnaryExpr:");
    log(ind + 1, "Operator: {}", node->oper.get());
    log(ind + 1, "Operand:");
    node->operand->accept(this, ind + 2);
}
void DumpVisitor::visit(AssignmentExpr* node, size_t ind)
{
    log(ind, "AssignmentExpr:");
    log(ind + 1, "Operator: {}", node->oper.get());
    log(ind + 1, "LHS:");
    node->lhs->accept(this, ind + 2);
    log(ind + 1, "RHS:");
    node->rhs->accept(this, ind + 2);
}
void DumpVisitor::visit(ArbitraryOperandExpr* node, size_t ind)
{
    log(ind, "ArbitraryOperandExpr:");
    log(ind + 1, "Operator: {}", node->oper.get());
    log(ind + 1, "Operands:");
    for(auto& o : node->operands)
    {
        o->accept(this, ind + 2);
    }
}

void DumpVisitor::visit(EmptyStmt* /*unused*/, size_t ind)
{
    log(ind, "EmptyStmt");
}
void DumpVisitor::visit(BlockStmt* node, size_t ind)
{
    log(ind, "BlockStmt:");
    auto& children = node->nodes;
    for(auto&& child : children)
    {
        child->accept(this, ind + 1);
    }
}
void DumpVisitor::visit(ExprStmt* node, size_t ind)
{
    log(ind, "ExprStmt:");
    node->expr->accept(this, ind + 1);
}
void DumpVisitor::visit(AliasStmt* node, size_t ind)
{
    log(ind, "AliasStmt:");
    log(ind + 1, "Alias: '{}'", node->alias->value);
    log(ind + 1, "Aliasee: '{}'", node->aliasee->value);
}
} // namespace ast
