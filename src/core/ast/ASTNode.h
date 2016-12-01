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

namespace core
{
	namespace ast
	{
		class ASTNode
		{
		public:
			virtual void accept(DumpASTVisitor *v, size_t ind = 0) = 0;

			ASTNode() = default;
			ASTNode(const ASTNode&) = default;
			ASTNode &operator = (const ASTNode&) = default;
			ASTNode(ASTNode&&) = default;
			ASTNode &operator = (ASTNode&&) = default;
			virtual ~ASTNode() = default;
		};
	}
}
