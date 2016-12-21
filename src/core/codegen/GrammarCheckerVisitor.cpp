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
		void GrammarCheckerVisitor::visit(ast::ASTStatement *node)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTIfStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTForStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTForeachStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTWhileStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTImportStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTModuleStatement *node)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTFunctionDefinitionStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTFunctionDeclarationStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTReturnStatement *node)
		{

		}

		void GrammarCheckerVisitor::visit(ast::ASTEmptyStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTBlockStatement *node)
		{

		}
		void GrammarCheckerVisitor::visit(ast::ASTWrappedExpressionStatement *node)
		{

		}
	}
}
