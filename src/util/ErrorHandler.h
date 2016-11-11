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

#include "util/SafeEnum.h"
#include "util/Logger.h"

namespace util
{
	enum Error_t
	{
		ERROR_DEFAULT = 0,
		WARNING_DEFAULT,

		ERROR_CLI_TCLAP_EXCEPTION = 100,
		ERROR_INPUT_FILE_READ_FAILED,
		ERROR_FILE_READING_LIBCPP,
		ERROR_FILE_READING_LIBC,

		WARNING_FILE_NOT_OPEN = 150,

		ERROR_LEXER_INVALID_INTEGER_LITERAL = 200,
		ERROR_LEXER_INVALID_FLOAT_LITERAL,
		ERROR_LEXER_FLOAT_LITERAL_HEX,
		ERROR_LEXER_INVALID_OPERATOR_OR_PUNCTUATOR,
		ERROR_LEXER_EMPTY_TOKEN_LIST,
		ERROR_LEXER_PARENTHESIS_MISMATCH,
		ERROR_LEXER_BRACKET_MISMATCH,
		ERROR_LEXER_BRACE_MISMATCH,
		ERROR_LEXER_INVALID_CHAR_LITERAL_NO_CLOSING_QUOTE,
		ERROR_LEXER_INVALID_STRING_LITERAL_NO_CLOSING_QUOTE,
		ERROR_LEXER_INVALID_CHAR_LITERAL_LENGTH_NOT_1,

		WARNING_LEXER_UNRECOGNIZED_CHARACTER = 350,
		WARNING_LEXER_UNRECOGNIZED_TOKEN,
		WARNING_LEXER_EMPTY_TRANSLATION_UNIT,
		WARNING_LEXER_STRING_INVALID_HEX_ESCAPE,
		WARNING_LEXER_STRING_INVALID_OCT_ESCAPE,
		WARNING_LEXER_STRING_INVALID_ESCAPE,

		// Parser error = 400
		// Parser warning = 550
		// Codegen/optimization error = 600
		// Codegen/optimization warning = 750
		// Interpreter error = 800
		// Interpreter warning = 850
		// Translation error = 900
		// Translation warning = 950

		ERROR_MAX = 1000
	};

	typedef SafeEnum<Error_t> Error;

	namespace ErrorHandler
	{
		std::string codeToString(Error code);
		bool isCodeError(Error code);

		template <typename... Args>
		inline void warn(Error code, const std::string &message, const Args& ... args)
		{

		}

		template <typename... Args>
		inline void error(Error code, const std::string &message, const Args& ... args)
		{

		}

		template <typename... Args>
		inline void generic(Error code, const std::string &message, const Args& ... args)
		{
			if(isCodeError(code))	error(code, message, args...);
			else					warn(code, message, args...);
		}
	};
}
