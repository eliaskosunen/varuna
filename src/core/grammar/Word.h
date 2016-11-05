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
#include <algorithm>
#include <cctype>

#include "core/grammar/Token.h"

namespace core
{
	namespace grammar
	{
		class Word : public TokenRule
		{
		public:
			Word() : TokenRule() {}
			virtual ~Word() {}

			virtual int identify(const std::string &buffer) override
			{
				return -1;
			}
			int validate(const std::string &buffer) override final
			{
				if(buffer.empty()) return -1;
				if(!(std::isalpha(buffer[0]) || buffer[0] == '_')) return 1;
				if(std::find_if(buffer.begin() + 1, buffer.end(), [](char ch)
				{
					return !(std::isalnum(ch) || ch == '_');
				}) != buffer.end())
				{
					return 1;
				}
				return 0;
			}
		};

		class WordKeyword : public Word
		{
		public:
			WordKeyword() : Word() {}

			int identify(const std::string &buffer) override final
			{
				if(buffer.empty()) return -1;
				if(
					// Control words, commands
					buffer == "import" || buffer == "function" || buffer == "declare" ||
					buffer == "export" || buffer == "class" || buffer == "override" ||
					buffer == "final" || buffer == "extend" || buffer == "abstract" ||
					buffer == "public" || buffer == "protected" || buffer == "private" ||
					buffer == "if" || buffer == "while" || buffer == "for" || buffer == "foreach" ||
					buffer == "switch" || buffer == "case" || buffer == "break" ||
					buffer == "return" || buffer == "continue" ||

					// Datatypes
					buffer == "None" || buffer == "Void" || buffer == "Integer" || buffer == "Float" ||
					buffer == "Double" || buffer == "Decimal" || buffer == "String" || buffer == "Char" ||
					buffer == "UInteger" || buffer == "BigInteger" || buffer == "Bool" ||
					buffer == "Int8" || buffer == "Int16" || buffer == "Int32" || buffer == "Int64" ||
					buffer == "UInt8" || buffer == "UInt16" || buffer == "UInt32" || buffer == "UInt64" ||
					buffer == "List" || buffer == "Dict" || buffer == "Array"
				)
				{
					return 0;
				}
				return 1;
			}
		};

		extern WordKeyword wordKeyword;

		class WordIdentifier : public Word
		{
		public:
			WordIdentifier() : Word() {}

			int identify(const std::string &buffer) override final
			{
				if(buffer.empty()) return -1;
				if(wordKeyword.identify(buffer) == 0) return 1;
				return 0;
			}
		};

		extern WordIdentifier wordIdentifier;
	}
}
