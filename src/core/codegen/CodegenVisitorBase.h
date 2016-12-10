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
#include "core/ast/Visitor.h"

#include "llvm/IR/Value.h"

namespace core
{
	namespace codegen
	{
		class CodegenVisitorBase : public ast::Visitor
		{
		public:
			CodegenVisitorBase() = default;

			CodegenVisitorBase(const CodegenVisitorBase&) = default;
			CodegenVisitorBase(CodegenVisitorBase&&) = default;

			virtual CodegenVisitorBase &operator =(const CodegenVisitorBase&) = default;
			virtual CodegenVisitorBase &operator =(CodegenVisitorBase&&) = default;

			virtual ~CodegenVisitorBase() = default;
		};
	}
}
