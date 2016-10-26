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

#include <string>
#include <vector>

#include "core/lexer/Token.h"

namespace core
{
	namespace lexer
	{
		typedef std::vector<Token> TokenVector;

		class Lexer
		{
			bool isKeyword(const std::string &buf);
			bool isArgOperator(const char &curr);
			bool isControlOperator(const char &curr);
			bool isTerminatingOperator(const char &curr);

		public:
			Lexer() {}

			TokenVector run(const std::string &str);
		};
	}
}
