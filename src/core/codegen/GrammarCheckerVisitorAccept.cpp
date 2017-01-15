// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/codegen/GrammarCheckerVisitor.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"

bool core::ast::ASTExpression::accept(core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTStatement::accept(core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool core::ast::ASTIfStatement::accept(core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTForStatement::accept(core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTForeachStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTWhileStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTImportStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTModuleStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool core::ast::ASTEmptyExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTIdentifierExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTVariableRefExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTCastExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTVariableDefinitionExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTSubscriptExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTSubscriptRangedExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTMemberAccessExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool core::ast::ASTFunctionPrototypeStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTFunctionDefinitionStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTReturnStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool core::ast::ASTIntegerLiteralExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTFloatLiteralExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTStringLiteralExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTCharLiteralExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTBoolLiteralExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTNoneLiteralExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool core::ast::ASTBinaryOperationExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTUnaryOperationExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTAssignmentOperationExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTArbitraryOperationExpression::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool core::ast::ASTEmptyStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTBlockStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool core::ast::ASTWrappedExpressionStatement::accept(
    core::codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
