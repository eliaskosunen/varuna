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

#include "core/ast/FwdDecl.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTExpression.h"

#include <vector>
#include <memory>

namespace core
{
	namespace ast
	{
		class ASTStatement : public ASTNode
		{
		public:
			virtual void accept(DumpASTVisitor *v, size_t ind = 0);

			ASTStatement() = default;
			ASTStatement(const ASTStatement&) = default;
			ASTStatement &operator = (const ASTStatement&) = default;
			ASTStatement(ASTStatement&&) = default;
			ASTStatement &operator = (ASTStatement&&) = default;
			virtual ~ASTStatement() = default;
		};

		class ASTEmptyStatement : public ASTStatement
		{
		public:
			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTBlockStatement : public ASTStatement
		{
		public:
			typedef std::unique_ptr<ASTStatement> Statement;
			typedef std::vector<Statement> StatementVector;
			StatementVector nodes;

			ASTBlockStatement() {}
			ASTBlockStatement(Statement first)
			{
				nodes.push_back(std::move(first));
			}
			ASTBlockStatement(StatementVector vec)
				: nodes(std::move(vec)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTWrappedExpressionStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> expr;

			ASTWrappedExpressionStatement(std::unique_ptr<ASTExpression> expression) : expr(std::move(expression)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};
	}
}
