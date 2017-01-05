/*
Copyright (C) 2016-2017 Elias Kosunen

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

#include "core/codegen/CodegenVisitor.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"

std::unique_ptr<core::codegen::TypedValue> core::ast::ASTExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue> core::ast::ASTIfStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTForStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTForeachStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTWhileStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTImportStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTModuleStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue> core::ast::ASTEmptyExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTIdentifierExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTVariableRefExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTCallExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTCastExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTVariableDefinitionExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTSubscriptExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTSubscriptRangedExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTMemberAccessExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue> core::ast::ASTFunctionParameter::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTFunctionPrototypeStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTFunctionDefinitionStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTFunctionDeclarationStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTReturnStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue> core::ast::ASTIntegerLiteralExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTFloatLiteralExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTStringLiteralExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTCharLiteralExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTBoolLiteralExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTNoneLiteralExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue> core::ast::ASTBinaryOperationExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTUnaryOperationExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTAssignmentOperationExpression::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}

std::unique_ptr<core::codegen::TypedValue> core::ast::ASTEmptyStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTBlockStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
std::unique_ptr<core::codegen::TypedValue> core::ast::ASTWrappedExpressionStatement::accept(core::codegen::CodegenVisitor *v)
{
	return v->visit(this);
}
