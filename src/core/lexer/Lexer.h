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
		/**
		 * Vector of tokens
		 */
		typedef std::vector<Token> TokenVector;

		/**
		 * Lexer class
		 */
		class Lexer
		{
			/**
			 * Is the given string a keyword
			 * @param  buf Current buffer
			 * @return     true = keyword
			 */
			bool isKeyword(const std::string &buf);
			/**
			 * Is the given string an entire operator
			 * @param  buf Current buffer
			 * @return     true = operator
			 */
			bool isWholeOperator(const std::string &buf);
			/**
			 * Is the current character a beginning of an operator
			 * @param  curr Current character
			 * @return      true = operator
			 */
			bool isBeginningOfOperator(const char &curr);
			/**
			 * Is the current character an operator that terminates a words
			 * @param  curr Current character
			 * @return      true = operator
			 */
			bool isTerminatingOperator(const char &curr);
			/**
			 * Is the given string an operator instead of a word
			 * @param  buf Current buffer
			 * @return     true = operator
			 */
			bool isWordOperator(const std::string &buf);
			/**
			 * Is the given string a boolean literal (true, false) instead of a word
			 * @param  buf Current buffer
			 * @return     true = boolean literal
			 */
			bool isBooleanLiteral(const std::string &buf);
			/**
			 * Is the given string a none literal (none) instead of a word
			 * @param  buf Current buffer
			 * @return     true = none literal
			 */
			bool isNoneLiteral(const std::string &buf);

		public:
			/**
			 * Lexer constructor
			 */
			Lexer() {}

			/**
			 * Run the lexer.
			 * @param  str      Preprocessed code
			 * @param  error    Reference, will be set to true in case of an error
			 * @param  filename Name of the current code file, used for error messages
			 * @return          Vector of parsed tokens
			 */
			TokenVector run(const std::string &str, bool &error, const std::string &filename = "undefined");
		};
	}
}
