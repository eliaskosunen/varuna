// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/CodegenVisitor.h"
#include "ast/ASTControlStatement.h"
#include "ast/ASTExpression.h"
#include "ast/ASTFunctionStatement.h"
#include "ast/ASTLiteralExpression.h"
#include "ast/ASTNode.h"
#include "ast/ASTOperatorExpression.h"
#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"

std::unique_ptr<codegen::TypedValue>
ast::ASTExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::ASTIfStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTForStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTForeachStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTWhileStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTImportStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTModuleStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::ASTEmptyExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTIdentifierExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTVariableRefExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTCastExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTVariableDefinitionExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTGlobalVariableDefinitionExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTSubscriptExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTSubscriptRangedExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTMemberAccessExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::ASTFunctionParameter::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTFunctionPrototypeStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTFunctionDefinitionStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTReturnStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::ASTIntegerLiteralExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTFloatLiteralExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTStringLiteralExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTCharLiteralExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTBoolLiteralExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTNoneLiteralExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::ASTBinaryOperationExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTUnaryOperationExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTAssignmentOperationExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTArbitraryOperationExpression::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::ASTEmptyStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTBlockStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ASTWrappedExpressionStatement::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
