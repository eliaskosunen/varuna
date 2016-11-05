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

#include "core/parser/Parser.h"

#include "core/parser/SyntaxTree.h"
#include "core/lexer/Lexer.h"
#include "core/lexer/Token.h"
#include "util/Compatibility.h"

namespace core
{
	namespace parser
	{
		SyntaxTree Parser::run(core::lexer::TokenVector &tokens)
		{
			SyntaxTree tree;
			core::lexer::TokenVector buffer;
			buffer.reserve(8);
			core::lexer::TokenVector::iterator tokenptr = tokens.begin();
			const core::lexer::TokenVector::const_iterator tokenendptr = tokens.end();

			for(; tokenptr != tokenendptr; ++tokenptr)
			{
				const core::lexer::Token &currentToken = *tokenptr;
			}

			return tree;
		}
	}
}
