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

#include <memory>
#include <vector>

#include "core/parser/FwdDecl.h"
#include "core/parser/TreeNode.h"

namespace core
{
	namespace parser
	{
		class Statement: public TreeNode
		{
		public:
			Statement();
			virtual ~Statement() {}

			void accept(Visitor &v) override;
		};

		typedef std::vector<std::unique_ptr<Statement>> StatementPtrVector;

		class StatementBlock: public Statement
		{
		protected:
			StatementPtrVector statementList;
		public:
			StatementBlock();
			virtual ~StatementBlock() {}

			void accept(Visitor &v) override;

			const StatementPtrVector &getStatementList() const { return statementList; }

			void pushStatement(std::unique_ptr<Statement> s)
			{
				statementList.push_back(std::move(s));
			}
		};

		class StatementEmpty: public Statement
		{
		public:
			StatementEmpty() : Statement() {}
		};
	}
}
