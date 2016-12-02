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
#include <map>

#include "core/lexer/TokenType.h"
#include "util/SafeEnum.h"
#include "util/Logger.h"

namespace core
{
	namespace lexer
	{
		enum TokenIntegerLiteralModifier_t
		{
			INTEGER_INTEGER	= 1,	// No suffix
			INTEGER_LONG	= 2,	// l
			INTEGER_SHORT	= 4,	// s
			INTEGER_BINARY	= 8,	// b
			INTEGER_OCTAL	= 16,	// o
			INTEGER_HEX		= 32,	// x

			INTEGER_NONE	= INTEGER_INTEGER
		};

		enum TokenFloatLiteralModifier_t
		{
			FLOAT_DOUBLE	= 1,	// No suffix
			FLOAT_FLOAT		= 2,	// f
			FLOAT_DECIMAL	= 4,	// d

			FLOAT_NONE		= FLOAT_DOUBLE
		};

		typedef util::SafeEnum<TokenIntegerLiteralModifier_t> TokenIntegerLiteralModifier;
		typedef util::SafeEnum<TokenFloatLiteralModifier_t> TokenFloatLiteralModifier;

		class SourceLocation
		{
		public:
			std::string file;
			uint64_t line, column;

			SourceLocation(const std::string &f = "undefined", uint64_t l = 0, uint64_t c = 0)
				: file(f), line(l), column(c) {}

			std::string toString() const
			{
				return fmt::format("{}:{}:{}", file, line, column);
			}
		};

		class Token
		{
		public:
			SourceLocation loc;

			TokenType type;
			std::string value;

			TokenIntegerLiteralModifier modifierInt;
			TokenFloatLiteralModifier modifierFloat;

			Token(TokenType t = TOKEN_DEFAULT, const std::string &val = "") : loc(), type(t), value(val), modifierInt(INTEGER_NONE), modifierFloat(FLOAT_NONE) {}

			std::string typeToString() const;

			static std::string typeToString(TokenType t)
			{
				Token tok(t, "");
				return tok.typeToString();
			}

			static Token create(TokenType t, const std::string &val, SourceLocation loc = SourceLocation())
			{
				Token tok(t, val);
				tok.loc = loc;
				return tok;
			}
		};
	}
}
