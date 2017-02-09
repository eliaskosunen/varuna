// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/GrammarCheckerVisitor.h"
#include "ast/ASTControlStatement.h"
#include "ast/ASTExpression.h"
#include "ast/ASTFunctionStatement.h"
#include "ast/ASTLiteralExpression.h"
#include "ast/ASTNode.h"
#include "ast/ASTOperatorExpression.h"
#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"

namespace codegen
{
bool GrammarCheckerVisitor::visit(ast::ASTStatement*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTExpression*)
{
    return true;
}

bool GrammarCheckerVisitor::visit(ast::ASTIfStatement*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTForStatement*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTForeachStatement*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTWhileStatement*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTImportStatement*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTModuleStatement*)
{
    return true;
}

bool GrammarCheckerVisitor::visit(ast::ASTEmptyExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTIdentifierExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTVariableRefExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTCastExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTVariableDefinitionExpression* node)
{
    /*if(node->type->value == "void")
    {
        return grammarError("Variable type cannot be void");
    }*/

    if(!node->type->accept(this))
    {
        return false;
    }
    if(!node->name->accept(this))
    {
        return false;
    }
    if(!node->init->accept(this))
    {
        return false;
    }
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTGlobalVariableDefinitionExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTSubscriptExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTSubscriptRangedExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTMemberAccessExpression*)
{
    return true;
}

bool GrammarCheckerVisitor::visit(ast::ASTFunctionPrototypeStatement*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTFunctionDefinitionStatement* node)
{
    if(!node->proto->accept(this))
    {
        return false;
    }
    return node->body->accept(this);
}
bool GrammarCheckerVisitor::visit(ast::ASTReturnStatement*)
{
    return true;
}

bool GrammarCheckerVisitor::visit(ast::ASTIntegerLiteralExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTFloatLiteralExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTStringLiteralExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTCharLiteralExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTBoolLiteralExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTNoneLiteralExpression*)
{
    return true;
}

bool GrammarCheckerVisitor::visit(ast::ASTBinaryExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTUnaryExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTAssignmentExpression*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTArbitraryOperandExpression*)
{
    return true;
}

bool GrammarCheckerVisitor::visit(ast::ASTEmptyStatement*)
{
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTBlockStatement* node)
{
    auto& children = node->nodes;
    for(auto&& child : children)
    {
        if(!child->accept(this))
        {
            return false;
        }
    }
    return true;
}
bool GrammarCheckerVisitor::visit(ast::ASTWrappedExpressionStatement* node)
{
    return node->expr->accept(this);
}
} // namespace codegen
