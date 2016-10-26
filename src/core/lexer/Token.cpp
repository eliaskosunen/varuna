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

#include "core/lexer/Token.h"

#include <string>

namespace core
{
	namespace lexer
	{
		const std::string Token::DEFAULT_PARAM = "";

		Token::Token(TokenType _type) : type(_type), value("") {}

		void Token::setType(TokenType _type)
		{
			type = _type;
		}

		TokenType Token::getType() const
		{
			return type;
		}

		std::string Token::typeToString() const
		{
			switch(type)
			{
			case TOKEN_DEFAULT:
				return "DEFAULT";
			case TOKEN_UNKNOWN:
				return "UNKNOWN";
			case TOKEN_IDENTIFIER:
				return "IDENTIFIER";
			case TOKEN_LITERAL_NUMBER:
				return "LITERAL_NUMBER";
			case TOKEN_ARG_OPERATOR:
				return "ARG_OPERATOR";
			case TOKEN_KEYWORD:
				return "KEYWORD";
			case TOKEN_CONTROL_OPERATOR:
				return "CONTROL_OPERATOR";
			case TOKEN_KEYWORD_OR_IDENTIFIER:
				return "KEYWORD/IDENTIFIER";
			}
			return "DEFAULT";
		}

		void Token::setValue(std::string val)
		{
			value = val;
		}

		const std::string &Token::getValue() const
		{
			return value;
		}

		const Token::ParamMap &Token::getParams() const
		{
			return params;
		}

		const std::string &Token::getParam(const std::string &key) const
		{
			if(!paramExists(key))
			{
				return DEFAULT_PARAM;
			}
			return params.at(key);
		}

		void Token::setParam(const std::string &key, const std::string value, bool override)
		{
			if(paramExists(key))
			{
				if(!override)
				{
					return;
				}
				removeParam(key);
			}
			params.insert(std::make_pair(key, value));
		}

		void Token::removeParam(const std::string &key)
		{
			if(!paramExists(key))
			{
				return;
			}
			params.erase(key);
		}

		bool Token::paramExists(const std::string &key) const
		{
			return (bool)params.count(key);
		}

		bool Token::isParamsEmpty() const
		{
			return params.empty();
		}

		void Token::clearParams()
		{
			params.clear();
		}

		void Token::reset()
		{
			setType(TOKEN_DEFAULT);
			setValue("");
			clearParams();
		}
	}
}
