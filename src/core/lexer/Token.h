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
		/**
		 * Token category
		 */
		enum TokenCategory
		{
			TOKEN_CAT_DEFAULT,	///< Default category
			TOKEN_CAT_UNKNOWN,	///< Unknown token

			TOKEN_CAT_WORD,		///< Word: keyword or identifier
			TOKEN_CAT_LITERAL,	///< Literal: numbers, strings...
			TOKEN_CAT_OPERATOR	///< Operator
		};

		enum TokenType
		{
			TOKEN_DEFAULT,					///< Default type
			TOKEN_UNKNOWN,					///< Unknown type

			TOKEN_WORD_DEFAULT,				///< Default (not yet defined/finished) word
			TOKEN_WORD_KEYWORD,				///< Keyword
			TOKEN_WORD_IDENTIFIER,			///< Identifier

			TOKEN_LITERAL_DEFAULT,			///< Default literal
			TOKEN_LITERAL_DEFAULT_NUMBER,	///< Default (not yet finished) number literal
			TOKEN_LITERAL_STRING,			///< String literal
			TOKEN_LITERAL_INTEGER,			///< Integer literal
			TOKEN_LITERAL_FLOAT,			///< Float literal
			TOKEN_LITERAL_CHAR,				///< Character literal
			TOKEN_LITERAL_BOOLEAN,			///< Boolean literal
			TOKEN_LITERAL_NONE,				///< None literal

			TOKEN_OPERATOR					///< Operator
		};

		/**
		 * Token class
		 */
		class Token
		{
		private:
			/**
			 * Category
			 */
			TokenCategory cat;
			/**
			 * Type
			 */
			TokenType type;

			/**
			 * Textual value
			 */
			std::string value;

			std::string file;

			unsigned int line;

			/**
			 * Get the default type of category
			 * @param  _cat Category
			 * @return      Type
			 */
			TokenType getTypeFromCategory(TokenCategory _cat) const;
			/**
			 * Get the category of type
			 * @param  _type Type
			 * @return       Category
			 */
			TokenCategory getCategoryFromType(TokenType _type) const;

		public:
			/**
			 * Default constructor
			 */
			Token() : Token(TOKEN_DEFAULT) {}
			Token(TokenCategory _cat);
			Token(TokenType _type);

			/**
			 * Set the category of the token.
			 * Does not set the type
			 * @param _cat Category
			 */
			void setCategory(TokenCategory _cat);
			/**
			 * Get the category of the token
			 * @return Category
			 */
			TokenCategory getCategory() const;

			/**
			 * Set the type of the token.
			 * Does not set the category
			 * @param _type Type
			 */
			void setType(TokenType _type);
			/**
			 * Get the type of the token
			 * @return Type
			 */
			TokenType getType() const;

			/**
			 * Set the textual value of the token
			 * @param val Value
			 */
			void setValue(std::string val);
			/**
			 * Get the textual value of the token
			 * @return Value
			 */
			const std::string &getValue() const;

			void setFile(std::string val);

			const std::string &getFile() const;

			void setLine(unsigned int val);

			unsigned int getLine();

			/**
			 * Get the category of the token as string
			 * @return Category as string
			 */
			std::string categoryToString() const;
			/**
			 * Get the type of the token as string
			 * @return Type as string
			 */
			std::string typeToString() const;

			/**
			 * Reset token to default values
			 */
			void reset();
		};
	}
}
