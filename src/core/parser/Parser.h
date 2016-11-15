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
#include "core/parser/AST.h"
#include "core/lexer/Lexer.h"
#include "util/Logger.h"
#include "util/Compatibility.h"

namespace core
{
	namespace parser
	{
		class Parser
		{
			std::unique_ptr<AST> ast;
		public:
			Parser() : ast(util::make_unique<AST>()) {}

			void run(const core::lexer::TokenVector &tokens);
			AST *getAST() { return ast.get(); }
			std::unique_ptr<AST> retrieveAST() { return std::move(ast); }
		};
	}
}
