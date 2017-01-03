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

#pragma once

#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/Visitor.h"

#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"

namespace core
{
	namespace codegen
	{
		class GrammarCheckerVisitor : public ast::Visitor
		{
		public:
			GrammarCheckerVisitor() = default;

			template <typename T>
			bool run(T root)
			{
				auto castedRoot = dynamic_cast<ast::ASTNode*>(root);
				if(!castedRoot)
				{
					throw std::invalid_argument("Invalid root node given to GrammarCheckerVisitor");
				}
				return root->accept(this);
			}

			template <typename... Args>
			bool grammarError(const std::string &format, Args... args) const
			{
				util::logger->error(format.c_str(), args...);
				return false;
			}

			template <typename... Args>
			void grammarWarning(const std::string &format, Args... args) const
			{
				util::logger->warn(format.c_str(), args...);
			}

			bool visit(ast::ASTNode *node) = delete;
			bool visit(ast::ASTStatement *node);
			bool visit(ast::ASTExpression *node);

			bool visit(ast::ASTIfStatement *node);
			bool visit(ast::ASTForStatement *node);
			bool visit(ast::ASTForeachStatement *node);
			bool visit(ast::ASTWhileStatement *node);
			bool visit(ast::ASTImportStatement *node);
			bool visit(ast::ASTModuleStatement *node);

			bool visit(ast::ASTEmptyExpression *node);
			bool visit(ast::ASTIdentifierExpression *node);
			bool visit(ast::ASTVariableRefExpression *node);
			bool visit(ast::ASTCallExpression *node);
			bool visit(ast::ASTCastExpression *node);
			bool visit(ast::ASTVariableDefinitionExpression *node);
			bool visit(ast::ASTSubscriptExpression *node);
			bool visit(ast::ASTSubscriptRangedExpression *node);
			bool visit(ast::ASTMemberAccessExpression *node);

			bool visit(ast::ASTFunctionParameter *node) = delete;
			bool visit(ast::ASTFunctionPrototypeStatement *node);
			bool visit(ast::ASTFunctionDefinitionStatement *node);
			bool visit(ast::ASTFunctionDeclarationStatement *node);
			bool visit(ast::ASTReturnStatement *node);

			bool visit(ast::ASTIntegerLiteralExpression *node);
			bool visit(ast::ASTFloatLiteralExpression *node);
			bool visit(ast::ASTStringLiteralExpression *node);
			bool visit(ast::ASTCharLiteralExpression *node);
			bool visit(ast::ASTBoolLiteralExpression *node);
			bool visit(ast::ASTNoneLiteralExpression *node);

			bool visit(ast::ASTBinaryOperationExpression *node);
			bool visit(ast::ASTUnaryOperationExpression *node);
			bool visit(ast::ASTAssignmentOperationExpression *node);

			bool visit(ast::ASTEmptyStatement *node);
			bool visit(ast::ASTBlockStatement *node);
			bool visit(ast::ASTWrappedExpressionStatement *node);
		};
	}
}
