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
#include "util/Logger.h"

namespace core
{
	namespace lexer
	{
		typedef std::vector<core::lexer::Token> TokenVector;

		enum ErrorLevel
		{
			ERROR_NONE = 1,
			ERROR_WARNING = 2,
			ERROR_ERROR = 3
		};

		class Lexer
		{
			const std::string &content;
			const std::string &filename;

			unsigned int currentLine;

			ErrorLevel error;

			TokenType getTokenTypeFromWord(const std::string &buf) const;
			Token getTokenFromWord(const std::string &buf) const;
			Token createToken(TokenType type, const std::string &val) const;

			Token getNextToken(std::string::const_iterator &it);

			std::string lexStringLiteral(std::string::const_iterator &it, const std::string::const_iterator &end, bool isChar = false);

			TokenType getTokenTypeFromOperator(const std::string &buf) const;
			Token getTokenFromOperator(const std::string &buf) const;

			template <typename... Args>
			void lexerError(const std::string &format, const Args& ... args)
			{
				error = ERROR_ERROR;
				util::logger->error("Lexer error: On file {}, line {}: {}", filename, currentLine, fmt::format(format, args...));
			}

			template <typename... Args>
			void lexerWarning(const std::string &format, const Args& ... args)
			{
				if(error != ERROR_ERROR) error = ERROR_WARNING;
				util::logger->warn("Lexer warning: On file {}, line {}: {}", filename, currentLine, fmt::format(format, args...));
			}
		public:
			bool warningsAsErrors;

			Lexer(const std::string &cont, const std::string &file = "(undefined)") : content(cont), filename(file), currentLine(1), error(ERROR_NONE), warningsAsErrors(false) {}

			TokenVector run();

			bool getError() const
			{
				if(error == ERROR_NONE) return false;
				if(warningsAsErrors) return true;
				if(error == ERROR_ERROR) return true;
				return false;
			}

			ErrorLevel getErrorLevel() const
			{
				return error;
			}
		};
	}
}
