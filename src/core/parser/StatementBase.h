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
			void accept(Visitor &v) override;
		};

		class StatementBlock: public Statement
		{
			std::vector<std::shared_ptr<Statement>> statementList;
		};
	}
}
