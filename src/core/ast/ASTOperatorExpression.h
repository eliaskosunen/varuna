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
#include "core/ast/ASTExpression.h"
#include "core/lexer/Token.h"

namespace core
{
	namespace ast
	{
		class ASTBinaryOperationExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTExpression> left, right;
			core::lexer::TokenType oper;

			ASTBinaryOperationExpression(std::unique_ptr<ASTExpression> l, std::unique_ptr<ASTExpression> r, core::lexer::TokenType o)
				: ASTExpression(BINARY_OPERATION_EXPR), left(std::move(l)), right(std::move(r)), oper(o) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
			void accept(ASTParentSolverVisitor *v, ASTNode *parent);
		};

		class ASTUnaryOperationExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTExpression> operand;
			core::lexer::TokenType oper;

			ASTUnaryOperationExpression(std::unique_ptr<ASTExpression> _operand, core::lexer::TokenType o)
				: ASTExpression(UNARY_OPERATION_EXPR), operand(std::move(_operand)), oper(o) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
			void accept(ASTParentSolverVisitor *v, ASTNode *parent);
		};

		class ASTAssignmentOperationExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> lval;
			std::unique_ptr<ASTExpression> rval;
			core::lexer::TokenType oper;

			ASTAssignmentOperationExpression(std::unique_ptr<ASTIdentifierExpression> l, std::unique_ptr<ASTExpression> r, core::lexer::TokenType o)
				: ASTExpression(ASSIGNMENT_OPERATION_EXPR), lval(std::move(l)), rval(std::move(r)), oper(o) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
			void accept(ASTParentSolverVisitor *v, ASTNode *parent);
		};
	}
}
