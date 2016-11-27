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
#include "core/parser/ASTNode.h"
#include "core/parser/ASTExpression.h"

#include <vector>
#include <memory>

namespace core
{
	namespace parser
	{
		class ASTStatement : public ASTNode
		{
		public:
			virtual ~ASTStatement() {}
		};

		class ASTEmptyStatement : public ASTStatement
		{

		};

		class ASTBlockStatement : public ASTStatement
		{
		public:
			typedef std::unique_ptr<ASTStatement> Statement;
			typedef std::vector<Statement> StatementVector;
			StatementVector nodes;

			ASTBlockStatement() {}
			explicit ASTBlockStatement(Statement first)
			{
				nodes.push_back(std::move(first));
			}
			explicit ASTBlockStatement(StatementVector vec)
				: nodes(std::move(vec)) {}
		};

		class ASTWrappedExpressionStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> expr;

			explicit ASTWrappedExpressionStatement(std::unique_ptr<ASTExpression> expression) : expr(std::move(expression)) {}
		};
	}
}
