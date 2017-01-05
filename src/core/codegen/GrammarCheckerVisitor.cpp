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

#include "core/codegen/GrammarCheckerVisitor.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"

namespace core
{
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
		bool GrammarCheckerVisitor::visit(ast::ASTCallExpression*)
		{
			return true;
		}
		bool GrammarCheckerVisitor::visit(ast::ASTCastExpression*)
		{
			return true;
		}
		bool GrammarCheckerVisitor::visit(ast::ASTVariableDefinitionExpression *node)
		{
			/*if(node->type->value == "void")
			{
				return grammarError("Variable type cannot be void");
			}*/

			if(!node->type->accept(this)) return false;
			if(!node->name->accept(this)) return false;
			if(!node->init->accept(this)) return false;
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
		bool GrammarCheckerVisitor::visit(ast::ASTFunctionDefinitionStatement *node)
		{
			if(!node->proto->accept(this)) return false;
			return node->body->accept(this);
		}
		bool GrammarCheckerVisitor::visit(ast::ASTFunctionDeclarationStatement*)
		{
			return true;
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

		bool GrammarCheckerVisitor::visit(ast::ASTBinaryOperationExpression*)
		{
			return true;
		}
		bool GrammarCheckerVisitor::visit(ast::ASTUnaryOperationExpression*)
		{
			return true;
		}
		bool GrammarCheckerVisitor::visit(ast::ASTAssignmentOperationExpression*)
		{
			return true;
		}

		bool GrammarCheckerVisitor::visit(ast::ASTEmptyStatement*)
		{
			return true;
		}
		bool GrammarCheckerVisitor::visit(ast::ASTBlockStatement *node)
		{
			auto &children = node->nodes;
			for(auto &&child : children)
			{
				if(!child->accept(this))
				{
					return false;
				}
			}
			return true;
		}
		bool GrammarCheckerVisitor::visit(ast::ASTWrappedExpressionStatement *node)
		{
			return node->expr->accept(this);
		}
	}
}
