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

void core::ast::ASTStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

void core::ast::ASTIfStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTForStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTForeachStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTWhileStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTImportStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTModuleStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

void core::ast::ASTFunctionDefinitionStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTFunctionDeclarationStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTReturnStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}

void core::ast::ASTEmptyStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTBlockStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
void core::ast::ASTWrappedExpressionStatement::accept(core::codegen::GrammarCheckerVisitor *v)
{
	return v->visit(this);
}
