// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/ASTControlStatement.h"
#include "ast/ASTExpression.h"
#include "ast/ASTFunctionStatement.h"
#include "ast/ASTLiteralExpression.h"
#include "ast/ASTNode.h"
#include "ast/ASTOperatorExpression.h"
#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"
#include "codegen/GrammarCheckerVisitor.h"

bool ast::ASTExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool ast::ASTIfStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTForStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTForeachStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTWhileStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTImportStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTModuleStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool ast::ASTEmptyExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTIdentifierExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTVariableRefExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTVariableDefinitionExpression::accept(
    codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTGlobalVariableDefinitionExpression::accept(
    codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTSubscriptExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTSubscriptRangedExpression::accept(
    codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTMemberAccessExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool ast::ASTFunctionPrototypeStatement::accept(
    codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTFunctionDefinitionStatement::accept(
    codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTReturnStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool ast::ASTIntegerLiteralExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTFloatLiteralExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTStringLiteralExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTCharLiteralExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTBoolLiteralExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTNoneLiteralExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool ast::ASTBinaryExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTUnaryExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTAssignmentExpression::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTArbitraryOperandExpression::accept(
    codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}

bool ast::ASTEmptyStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTBlockStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTWrappedExpressionStatement::accept(
    codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
bool ast::ASTAliasStatement::accept(codegen::GrammarCheckerVisitor* v)
{
    return v->visit(this);
}
