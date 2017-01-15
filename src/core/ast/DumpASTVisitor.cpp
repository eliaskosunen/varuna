// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/ast/DumpASTVisitor.h"
#include "core/ast/AST.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"
#include "util/Logger.h"
#include <string>

namespace core
{
namespace ast
{
    void ASTStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }

    void ASTExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }

    void ASTIfStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTForStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTForeachStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTWhileStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTImportStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTModuleStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }

    void ASTEmptyExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTIdentifierExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTVariableRefExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTCastExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTVariableDefinitionExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTSubscriptExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTSubscriptRangedExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTMemberAccessExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }

    void ASTFunctionParameter::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTFunctionPrototypeStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTFunctionDefinitionStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTReturnStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }

    void ASTIntegerLiteralExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTFloatLiteralExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTStringLiteralExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTCharLiteralExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTBoolLiteralExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTNoneLiteralExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }

    void ASTBinaryOperationExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTUnaryOperationExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTAssignmentOperationExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTArbitraryOperationExpression::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }

    void ASTEmptyStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTBlockStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }
    void ASTWrappedExpressionStatement::accept(DumpASTVisitor* v, size_t ind)
    {
        v->visit(this, ind);
    }

    void DumpASTVisitor::visit(ASTStatement* /*unused*/, size_t ind)
    {
        log(ind, "ASTStatement");
    }

    void DumpASTVisitor::visit(ASTExpression* /*unused*/, size_t ind)
    {
        log(ind, "ASTExpression");
    }

    void DumpASTVisitor::visit(ASTIfStatement* node, size_t ind)
    {
        log(ind, "ASTIfStatement:");
        log(ind + 1, "Condition:");
        node->condition->accept(this, ind + 2);
        log(ind + 1, "IfBlock:");
        node->ifBlock->accept(this, ind + 2);
        log(ind + 1, "ElseBlock:");
        node->elseBlock->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTForStatement* node, size_t ind)
    {
        log(ind, "ASTForStatement:");

        log(ind + 1, "InitExpression:");
        node->init->accept(this, ind + 2);

        log(ind + 1, "EndExpression:");
        node->end->accept(this, ind + 2);

        log(ind + 1, "StepExpression:");
        node->step->accept(this, ind + 2);

        log(ind + 1, "Block:");
        node->block->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTForeachStatement* node, size_t ind)
    {
        log(ind, "ASTForeachStatement:");
        log(ind + 1, "Iteratee:");
        node->iteratee->accept(this, ind + 2);
        log(ind + 1, "Iterator:");
        node->iterator->accept(this, ind + 2);
        log(ind + 1, "Block:");
        node->block->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTWhileStatement* node, size_t ind)
    {
        log(ind, "ASTWhileStatement:");
        log(ind + 1, "Condition:");
        node->condition->accept(this, ind + 2);
        log(ind + 1, "Block:");
        node->block->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTImportStatement* node, size_t ind)
    {
        log(ind, "ASTImportStatement:");
        node->importee->accept(this, ind + 1);
        log(ind + 1, "isPath: {}", node->isPath);
        log(ind + 1, "importType: {}", node->importType);
    }
    void DumpASTVisitor::visit(ASTModuleStatement* node, size_t ind)
    {
        log(ind, "ASTModuleStatement:");
        node->moduleName->accept(this, ind + 1);
    }

    void DumpASTVisitor::visit(ASTEmptyExpression* /*unused*/, size_t ind)
    {
        log(ind, "ASTEmptyExpression");
    }
    void DumpASTVisitor::visit(ASTIdentifierExpression* node, size_t ind)
    {
        log(ind, "ASTIdentifierExpression: {}", node->value);
    }
    void DumpASTVisitor::visit(ASTVariableRefExpression* node, size_t ind)
    {
        log(ind, "ASTVariableRefExpression: {}", node->value);
    }
    void DumpASTVisitor::visit(ASTCastExpression* node, size_t ind)
    {
        log(ind, "ASTCastExpression:");
        log(ind + 1, "Type:");
        node->type->accept(this, ind + 2);
        log(ind + 1, "Castee:");
        node->castee->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTVariableDefinitionExpression* node,
                               size_t ind)
    {
        log(ind, "ASTVariableDefinitionExpression:");
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
        log(ind + 1, "ArraySize: {}", node->arraySize);
        log(ind + 1, "InitExpression:");
        node->init->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTSubscriptExpression* node, size_t ind)
    {
        log(ind, "ASTSubscriptExpression:");
        log(ind + 1, "LHS:");
        node->lhs->accept(this, ind + 2);
        log(ind + 1, "Index:");
        node->index->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTSubscriptRangedExpression* node, size_t ind)
    {
        log(ind, "ASTSubscriptRangedExpression");
    }
    void DumpASTVisitor::visit(ASTMemberAccessExpression* node, size_t ind)
    {
        log(ind, "ASTMemberAccessExpression:");
        log(ind + 1, "LHS:");
        node->lhs->accept(this, ind + 2);
        log(ind + 1, "RHS:");
        node->rhs->accept(this, ind + 2);
    }

    void DumpASTVisitor::visit(ASTFunctionParameter* node, size_t ind)
    {
        log(ind, "ASTFunctionParameter:");
        node->var->accept(this, ind + 1);
    }
    void DumpASTVisitor::visit(ASTFunctionPrototypeStatement* node, size_t ind)
    {
        log(ind, "ASTFunctionPrototypeStatement:");
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
    void DumpASTVisitor::visit(ASTFunctionDefinitionStatement* node, size_t ind)
    {
        log(ind, "ASTFunctionDefinitionStatement:");
        log(ind + 1, "FunctionPrototype:");
        node->proto->accept(this, ind + 2);
        log(ind + 1, "FunctionBody:");
        node->body->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTReturnStatement* node, size_t ind)
    {
        log(ind, "ASTReturnStatement:");
        node->returnValue->accept(this, ind + 1);
    }

    void DumpASTVisitor::visit(ASTIntegerLiteralExpression* node, size_t ind)
    {
        log(ind, "ASTIntegerLiteralExpression:");
        log(ind + 1, "Value: {}", node->value);
        log(ind + 1, "Type: {}", node->type->value);
    }
    void DumpASTVisitor::visit(ASTFloatLiteralExpression* node, size_t ind)
    {
        log(ind, "ASTFloatLiteralExpression:");
        log(ind + 1, "Value: {}", node->value);
        log(ind + 1, "Type: {}", node->type->value);
    }
    void DumpASTVisitor::visit(ASTStringLiteralExpression* node, size_t ind)
    {
        log(ind, "ASTStringLiteralExpression:");
        log(ind + 1, "Value: {}", node->value);
        log(ind + 1, "Type: {}", node->type->value);
    }
    void DumpASTVisitor::visit(ASTCharLiteralExpression* node, size_t ind)
    {
        log(ind, "ASTCharLiteralExpression:");
        log(ind + 1, "Value: {}", node->value);
        log(ind + 1, "Type: {}", node->type->value);
    }
    void DumpASTVisitor::visit(ASTBoolLiteralExpression* node, size_t ind)
    {
        log(ind, "ASTBoolLiteralExpression:");
        log(ind + 1, "Value: {}", node->value);
        log(ind + 1, "Type: {}", node->type->value);
    }
    void DumpASTVisitor::visit(ASTNoneLiteralExpression* node, size_t ind)
    {
        log(ind, "ASTNoneLiteralExpression");
        log(ind + 1, "Type: {}", node->type->value);
    }

    void DumpASTVisitor::visit(ASTBinaryOperationExpression* node, size_t ind)
    {
        log(ind, "ASTBinaryOperationExpression:");
        log(ind + 1, "Operator: {} ({})",
            core::lexer::Token::typeToString(node->oper), node->oper);
        log(ind + 1, "LHS:");
        node->lhs->accept(this, ind + 2);
        log(ind + 1, "RHS:");
        node->rhs->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTUnaryOperationExpression* node, size_t ind)
    {
        log(ind, "ASTUnaryOperationExpression:");
        log(ind + 1, "Operator: {} ({})",
            core::lexer::Token::typeToString(node->oper), node->oper);
        log(ind + 1, "Operand:");
        node->operand->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTAssignmentOperationExpression* node,
                               size_t ind)
    {
        log(ind, "ASTAssignmentOperationExpression:");
        log(ind + 1, "Operator: {} ({})",
            core::lexer::Token::typeToString(node->oper), node->oper);
        log(ind + 1, "LHS:");
        node->lhs->accept(this, ind + 2);
        log(ind + 1, "RHS:");
        node->rhs->accept(this, ind + 2);
    }
    void DumpASTVisitor::visit(ASTArbitraryOperationExpression* node,
                               size_t ind)
    {
        log(ind, "ASTArbitraryOperationExpression:");
        log(ind + 1, "Operator: {}, ({})",
            lexer::Token::typeToString(node->oper), node->oper);
        log(ind + 1, "Operands:");
        for(auto& o : node->operands)
        {
            o->accept(this, ind + 2);
        }
    }

    void DumpASTVisitor::visit(ASTEmptyStatement* /*unused*/, size_t ind)
    {
        log(ind, "ASTEmptyStatement");
    }
    void DumpASTVisitor::visit(ASTBlockStatement* node, size_t ind)
    {
        log(ind, "ASTBlockStatement:");
        auto& children = node->nodes;
        for(auto&& child : children)
        {
            child->accept(this, ind + 1);
        }
    }
    void DumpASTVisitor::visit(ASTWrappedExpressionStatement* node, size_t ind)
    {
        log(ind, "ASTWrappedExpressionStatement:");
        node->expr->accept(this, ind + 1);
    }
} // namespace ast
} // namespace core
