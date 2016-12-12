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

#include "core/codegen/Codegen.h"

namespace core
{
	namespace codegen
	{
		Codegen::Codegen(std::unique_ptr<ast::AST> a)
			: ast(std::move(a)), codegen(std::make_unique<CodegenVisitor>()) {}

		bool Codegen::run()
		{
			if(!prepare())	return false;
			if(!visit())	return false;
			if(!finish())	return false;
			return true;
		}

		bool Codegen::prepare()
		{
			
			return true;
		}

		bool Codegen::visit()
		{
			if(!codegen->codegen(ast->globalNode.get()))
			{
				return false;
			}
			return true;
		}

		bool Codegen::finish()
		{
			util::logger->trace("Module dump:");
			codegen->dumpModule();
			return true;
		}
	}
}
