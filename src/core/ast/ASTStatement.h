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

#include "core/ast/ASTExpression.h"
#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"

#include <memory>
#include <vector>

namespace core
{
	namespace ast
	{
		class ASTStatement : public ASTNode
		{
		protected:
			ASTStatement(NodeType t) : ASTNode(t) {}

		public:
			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			virtual llvm::Value *accept(codegen::CodegenVisitor *v);
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;

			ASTStatement() : ASTNode(STMT) {}
			ASTStatement(const ASTStatement&) = delete;
			ASTStatement &operator = (const ASTStatement&) = delete;
			ASTStatement(ASTStatement&&) = default;
			ASTStatement &operator = (ASTStatement&&) = default;
			~ASTStatement() override = default;
		};

		class ASTEmptyStatement : public ASTStatement
		{
		public:
			ASTEmptyStatement() : ASTStatement(EMPTY_STMT) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Value *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};

		class ASTBlockStatement : public ASTStatement
		{
		public:
			using Statement = std::unique_ptr<ASTStatement>;
			using StatementVector = std::vector<Statement>;
			StatementVector nodes;

			ASTBlockStatement() : ASTStatement(BLOCK_STMT) {}
			ASTBlockStatement(Statement first) : ASTStatement(BLOCK_STMT)
			{
				nodes.push_back(std::move(first));
			}
			ASTBlockStatement(StatementVector vec)
				: ASTStatement(BLOCK_STMT), nodes(std::move(vec)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Value *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};

		class ASTWrappedExpressionStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> expr;

			ASTWrappedExpressionStatement(std::unique_ptr<ASTExpression> expression)
				: ASTStatement(WRAPPED_EXPR_STMT), expr(std::move(expression)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Value *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};
	} // namespace ast
} // namespace core
