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

#include "core/ast/ASTParentSolverVisitor.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"

namespace core
{
	namespace ast
	{
		void ASTParentSolverVisitor::visit(ASTNode *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}

		void ASTParentSolverVisitor::visit(ASTIfStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTForStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTForeachStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTWhileStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTImportStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTModuleStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}

		void ASTParentSolverVisitor::visit(ASTEmptyExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTIdentifierExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTVariableRefExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTCallExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTCastExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTVariableDefinitionExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}

		void ASTParentSolverVisitor::visit(ASTFunctionParameter *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTFunctionPrototypeStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTFunctionDefinitionStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTFunctionDeclarationStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTReturnStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}

		void ASTParentSolverVisitor::visit(ASTIntegerLiteralExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTFloatLiteralExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTStringLiteralExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTCharLiteralExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTBoolLiteralExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTNoneLiteralExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}

		void ASTParentSolverVisitor::visit(ASTBinaryOperationExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTUnaryOperationExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTAssignmentOperationExpression *node, ASTNode *parent)
		{
			node->parent = parent;
		}

		void ASTParentSolverVisitor::visit(ASTEmptyStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTBlockStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
		void ASTParentSolverVisitor::visit(ASTWrappedExpressionStatement *node, ASTNode *parent)
		{
			node->parent = parent;
		}
	}
}

void core::ast::ASTExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}

void core::ast::ASTIfStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTForStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTForeachStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTWhileStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTImportStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTModuleStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}

void core::ast::ASTEmptyExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTIdentifierExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTVariableRefExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTCallExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTCastExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTVariableDefinitionExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}

void core::ast::ASTFunctionParameter::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTFunctionPrototypeStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTFunctionDefinitionStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTFunctionDeclarationStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTReturnStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}

void core::ast::ASTIntegerLiteralExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTFloatLiteralExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTStringLiteralExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTCharLiteralExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTBoolLiteralExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTNoneLiteralExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}

void core::ast::ASTBinaryOperationExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTUnaryOperationExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTAssignmentOperationExpression::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}

void core::ast::ASTEmptyStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTBlockStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
void core::ast::ASTWrappedExpressionStatement::accept(core::ast::ASTParentSolverVisitor *v, core::ast::ASTNode *p)
{
	return v->visit(this, p);
}
