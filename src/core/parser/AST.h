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
#include "core/parser/ASTStatement.h"
#include "core/parser/Visitor.h"
#include "util/Compatibility.h"

#include <algorithm>

namespace core
{
	namespace parser
	{
		class AST
		{
		public:
			std::unique_ptr<ASTBlockStatement> globalNode;

			AST() : globalNode(std::make_unique<ASTBlockStatement>()) {}

			void pushStatement(std::unique_ptr<ASTStatement> stmt)
			{
				globalNode->nodes.push_back(std::move(stmt));
			}

			void pushExpression(std::unique_ptr<ASTExpression> expr)
			{
				auto stmt = std::make_unique<ASTWrappedExpressionStatement>(std::move(expr));
				pushStatement(std::move(stmt));
			}

			int countTopLevelNodes()
			{
				return globalNode->nodes.size();
			}
		};
	}
}
