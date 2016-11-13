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

#include "util/ErrorHandler.h"

#include <string>

namespace util
{
	namespace ErrorHandler
	{
		static std::string codeToStringLexer(Error code)
		{
			switch(code.get())
			{
			case ERROR_LEXER_INVALID_INTEGER_LITERAL:
				return "Invalid integer literal: '{}'";
			case ERROR_LEXER_INVALID_FLOAT_LITERAL:
				return "Invalid floating point literal: '{}'";
			case ERROR_LEXER_FLOAT_LITERAL_HEX:
				return "Floating point literal cannot be in hexadecimal: '{}'";
			case ERROR_LEXER_EMPTY_TOKEN_LIST:
				return "Token list is empty after lexical analysis: No EOF found";
			case ERROR_LEXER_PARENTHESIS_MISMATCH:
				return "Syntax Error: Parenthesis mismatch";
			case ERROR_LEXER_BRACKET_MISMATCH:
				return "Syntax Error: Bracket mismatch";
			case ERROR_LEXER_BRACE_MISMATCH:
				return "Syntax Error: Brace mismatch";
			case ERROR_LEXER_INVALID_CHAR_LITERAL_NO_CLOSING_QUOTE:
				return "Invalid character literal: No closing quote found";
			case ERROR_LEXER_INVALID_STRING_LITERAL_NO_CLOSING_QUOTE:
				return "Invalid string literal: No closing quote found";
			case ERROR_LEXER_INVALID_CHAR_LITERAL_LENGTH_NOT_1:
				return "Invalid character literal: Length is not 1";
			default:
				throw(code);
			}
		}

		std::string codeToString(Error code)
		{
			return fmt::format("Lexer Error #{}: On {}:{}: ", code.get()) + codeToStringLexer(code);
		}

		bool isCodeError(Error code)
		{
			if(isCodeInfo(code))	return false;
			if(isCodeWarn(code))	return false;
			return true;
		}

		bool isCodeWarn(Error code)
		{
			if(isCodeInfo(code))	return false;
			if(code.get() == 1)		return true;
			if(code.get() >= 150 &&
				code.get() < 200)	return true;
			if(code.get() >= 350 &&
				code.get() < 400)	return true;
			if(code.get() >= 750 &&
				code.get() < 800)	return true;
			if(code.get() >= 850 &&
				code.get() < 900)	return true;
			if(code.get() >= 950 &&
				code.get() < 1000)	return true;
			return false;
		}

		bool isCodeInfo(Error code)
		{
			if(code.get() > 1000 || code.get() == 2)
			{
				return true;
			}
			return false;
		}
	}
}
