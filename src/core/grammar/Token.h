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
#include <locale>

namespace core
{
	namespace grammar
	{
		class TokenRule
		{
		public:
			TokenRule() {}
			virtual ~TokenRule() {}

			virtual int validate(const std::string &buffer) = 0;
			virtual int identify(const std::string &buffer) = 0;

			virtual bool allowWhitespaceBefore()
			{
				return true;
			}
			virtual bool allowWhitespacesAfter()
			{
				return true;
			}
			virtual bool terminate(const char &curr, const char &prev = 0)
			{
				if(
					curr == '{' || curr == '}' || curr == '(' || curr == ')' ||
					curr == '[' || curr == ']' || curr == '"' ||
					curr == ':' || curr == ';' || curr == '.' || curr == ',' ||

					std::isspace(curr)
				)
				{
					return true;
				}
				return false;
			}
		};
	}
}
