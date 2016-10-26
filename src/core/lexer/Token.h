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

namespace core
{
	namespace lexer
	{
		enum TokenType
		{
			TOKEN_DEFAULT,

			TOKEN_UNKNOWN,			/// Unknown
			TOKEN_IDENTIFIER,		/// Variables, functions etc.
			TOKEN_LITERAL_NUMBER,	/// Number literals
			TOKEN_ARG_OPERATOR,	/// Operators with arguments: + - * / etc.
			TOKEN_KEYWORD,			/// Reserved words: if while function etc.
			TOKEN_CONTROL_OPERATOR,	/// Operators without arguments: { } [ ]

			TOKEN_KEYWORD_OR_IDENTIFIER	/// Used by Lexer
		};

		class Token
		{
		protected:
			TokenType type;

			typedef std::map<std::string, std::string> ParamMap;
			ParamMap params;

			std::string value;

		public:
			const static std::string DEFAULT_PARAM;

			Token(TokenType _type = TOKEN_DEFAULT);

			void setType(TokenType _type);
			TokenType getType() const;

			void setValue(std::string val);
			const std::string &getValue() const;

			const ParamMap &getParams() const;
			const std::string &getParam(const std::string &key) const;
			void setParam(const std::string &key, const std::string value, bool override = true);
			void removeParam(const std::string &key);
			bool paramExists(const std::string &key) const;
			bool isParamsEmpty() const;
			void clearParams();

			std::string typeToString() const;

			void reset();
		};
	}
}
