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

		class ASTBlockStatement : public ASTStatement
		{
		public:
			std::vector<std::unique_ptr<ASTStatement>> nodes;

			ASTBlockStatement() {}
			explicit ASTBlockStatement(std::unique_ptr<ASTStatement> first)
			{
				nodes.push_back(std::move(first));
			}
			explicit ASTBlockStatement(std::vector<std::unique_ptr<ASTStatement>> &vec)
				: nodes(std::move(vec)) {}
		};

		class ASTVariableDefinitionStatement : public ASTStatement
		{
		public:
			std::unique_ptr<Identifier> type, name;

			ASTVariableDefinitionStatement(std::unique_ptr<Identifier> _type, std::unique_ptr<Identifier> _name)
				: type(std::move(_type)), name(std::move(_name)) {}
		};
	}
}
