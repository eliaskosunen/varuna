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

#include <memory>
#include <vector>

namespace core
{
	namespace parser
	{
		class ASTExpression : public ASTNode
		{
		public:
			virtual ~ASTExpression() {}
		};

		class ASTIdentifierExpression : public ASTExpression
		{
		public:
			std::string value;

			explicit ASTIdentifierExpression(std::string val) : value(val) {}
			explicit ASTIdentifierExpression(const std::string &val) : value(val) {}
		};

		class ASTCallExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTExpression> callee;
			std::vector<std::unique_ptr<ASTExpression>> params;

			explicit ASTCallExpression(std::unique_ptr<ASTExpression> _callee, std::vector<std::unique_ptr<ASTExpression>> _params)
				: callee(std::move(_callee)), params(std::move(_params)) {}
		};

		class ASTCastExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> type, castee;

			ASTCastExpression(std::unique_ptr<ASTIdentifierExpression> _castee, std::unique_ptr<ASTIdentifierExpression> _type)
				: type(std::move(_type)), castee(std::move(_castee)) {}
		};
	}
}
