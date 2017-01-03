/*
Copyright (C) 2016 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "core/codegen/GrammarCheckerVisitor.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"

bool core::ast::ASTExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

bool core::ast::ASTIfStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTForStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTForeachStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTWhileStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTImportStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTModuleStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

bool core::ast::ASTEmptyExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTIdentifierExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTVariableRefExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTCallExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTCastExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTVariableDefinitionExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTSubscriptExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTSubscriptRangedExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTMemberAccessExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

bool core::ast::ASTFunctionPrototypeStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTFunctionDefinitionStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTFunctionDeclarationStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTReturnStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

bool core::ast::ASTIntegerLiteralExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTFloatLiteralExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTStringLiteralExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTCharLiteralExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTBoolLiteralExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTNoneLiteralExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

bool core::ast::ASTBinaryOperationExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTUnaryOperationExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTAssignmentOperationExpression::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

bool core::ast::ASTEmptyStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTBlockStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
bool core::ast::ASTWrappedExpressionStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
