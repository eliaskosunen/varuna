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

namespace core
{
	namespace lexer
	{
		enum TokenIntegerLiteralModifier_t
		{
			INTEGER_INTEGER	= 1,	// No suffix
			INTEGER_UNSIGNED= 2,	// u
			INTEGER_LONG	= 4,	// l
			INTEGER_SHORT	= 8,	// s
			INTEGER_BINARY	= 16,	// b
			INTEGER_OCTAL	= 32,	// o
			INTEGER_HEX		= 64,	// x

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

		class Token
		{
		public:
			std::string file;
			unsigned long line;

			TokenType type;
			std::string value;

			TokenIntegerLiteralModifier modifierInt;
			TokenFloatLiteralModifier modifierFloat;

			Token(TokenType t = TOKEN_DEFAULT, const std::string &val = "") : file("(undefined)"), line(0), type(t), value(val), modifierInt(INTEGER_NONE), modifierFloat(FLOAT_NONE) {}

			std::string typeToString() const;

			static std::string typeToString(TokenType t)
			{
				Token tok(t, "");
				return tok.typeToString();
			}

			static Token create(TokenType t, const std::string &val, unsigned int l = 0, const std::string &f = "(undefined)")
			{
				Token tok(t, val);
				tok.file = f;
				tok.line = l;
				return tok;
			}
		};
	}
}
