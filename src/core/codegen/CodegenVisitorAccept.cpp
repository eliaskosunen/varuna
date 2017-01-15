// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/codegen/CodegenVisitor.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"

std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTIfStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTForStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTForeachStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTWhileStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTImportStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTModuleStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTEmptyExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTIdentifierExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTVariableRefExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTCastExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTVariableDefinitionExpression::accept(
    core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTSubscriptExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTSubscriptRangedExpression::accept(
    core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTMemberAccessExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTFunctionParameter::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTFunctionPrototypeStatement::accept(
    core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTFunctionDefinitionStatement::accept(
    core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTReturnStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTIntegerLiteralExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTFloatLiteralExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTStringLiteralExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTCharLiteralExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTBoolLiteralExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTNoneLiteralExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTBinaryOperationExpression::accept(
    core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTUnaryOperationExpression::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTAssignmentOperationExpression::accept(
    core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTArbitraryOperationExpression::accept(
    core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTEmptyStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTBlockStatement::accept(core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue>
core::ast::ASTWrappedExpressionStatement::accept(
    core::codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
