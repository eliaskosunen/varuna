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

namespace core
{
	namespace codegen
	{
		void GrammarCheckerVisitor::visit(ast::ASTStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTExpression*)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTIfStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTForStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTForeachStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTWhileStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTImportStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTModuleStatement*)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTEmptyExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTIdentifierExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTVariableRefExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTCallExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTCastExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTVariableDefinitionExpression*)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTFunctionPrototypeStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTFunctionDefinitionStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTFunctionDeclarationStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTReturnStatement*)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTIntegerLiteralExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTFloatLiteralExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTStringLiteralExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTCharLiteralExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTBoolLiteralExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTNoneLiteralExpression*)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTBinaryOperationExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTUnaryOperationExpression*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTAssignmentOperationExpression*)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTEmptyStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTBlockStatement*)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTWrappedExpressionStatement*)
		{

		}
	}
}
