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
#include "core/ast/FwdDecl.h"
#include "core/ast/ASTOperatorExpression.h"

#include "core/lexer/Token.h"
#include "util/SafeEnum.h"

namespace core
{
	namespace ast
	{
		class ASTIntegerLiteralExpression : public ASTExpression
		{
		public:
			int64_t value;
			std::unique_ptr<ASTIdentifierExpression> type;

			ASTIntegerLiteralExpression(int64_t val, std::unique_ptr<ASTIdentifierExpression> _type)
				: ASTExpression(INTEGER_LITERAL_EXPR), value(val), type(std::move(_type)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Constant *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};

		class ASTFloatLiteralExpression : public ASTExpression
		{
		public:
			double value;
			std::unique_ptr<ASTIdentifierExpression> type;

			ASTFloatLiteralExpression(double val, std::unique_ptr<ASTIdentifierExpression> _type)
				: ASTExpression(FLOAT_LITERAL_EXPR), value(val), type(std::move(_type)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Constant *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};

		class ASTStringLiteralExpression : public ASTExpression
		{
		public:
			const std::string &value;
			std::unique_ptr<ASTIdentifierExpression> type;

			ASTStringLiteralExpression(const std::string &val)
				: ASTExpression(STRING_LITERAL_EXPR), value(val), type(std::make_unique<ASTIdentifierExpression>("string")) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Constant *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};

		class ASTCharLiteralExpression : public ASTExpression
		{
		public:
			char32_t value;
			std::unique_ptr<ASTIdentifierExpression> type;

			ASTCharLiteralExpression(char32_t val)
				: ASTExpression(CHAR_LITERAL_EXPR), value(val), type(std::make_unique<ASTIdentifierExpression>("char")) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Constant *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};

		class ASTBoolLiteralExpression : public ASTExpression
		{
		public:
			bool value;
			std::unique_ptr<ASTIdentifierExpression> type;

			ASTBoolLiteralExpression(bool val)
				: ASTExpression(BOOL_LITERAL_EXPR), value(val), type(std::make_unique<ASTIdentifierExpression>("bool")) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Constant *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};

		class ASTNoneLiteralExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> type;

			ASTNoneLiteralExpression()
				: ASTExpression(NONE_LITERAL_EXPR), type(std::make_unique<ASTIdentifierExpression>("none_t")) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			llvm::Constant *accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};
	} // namespace ast
} // namespace core
