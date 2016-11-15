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

#include "core/parser/FwdDecl.h"
#include "core/parser/ASTExpression.h"
#include "core/lexer/Token.h"

namespace core
{
	namespace parser
	{
		class ASTBinaryOperationExpression : public ASTExpression
		{
			std::unique_ptr<ASTExpression> left, right;
			core::lexer::TokenType oper;
		public:
			ASTBinaryOperationExpression(std::unique_ptr<ASTExpression> l, std::unique_ptr<ASTExpression> r, core::lexer::TokenType o)
				: left(std::move(l)), right(std::move(r)), oper(o) {}
		};

		class ASTUnaryOperationExpression : public ASTExpression
		{
			std::unique_ptr<ASTExpression> operand;
			core::lexer::TokenType oper;
		public:
			ASTUnaryOperationExpression(std::unique_ptr<ASTExpression> _operand, core::lexer::TokenType o)
				: operand(std::move(_operand)), oper(o) {}
		};

		class ASTAssignmentOperationExpression : public ASTExpression
		{
			std::unique_ptr<Identifier> lval;
			std::unique_ptr<ASTExpression> rval;
			core::lexer::TokenType oper;
		public:
			ASTAssignmentOperationExpression(std::unique_ptr<Identifier> l, std::unique_ptr<ASTExpression> r, core::lexer::TokenType o)
				: lval(std::move(l)), rval(std::move(r)), oper(o) {}
		};
	}
}
