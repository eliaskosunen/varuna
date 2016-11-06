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
		enum TokenType : unsigned char
		{
			TOKEN_DEFAULT = 0,

			TOKEN_KEYWORD_IMPORT,
			TOKEN_KEYWORD_FUNCTION,
			TOKEN_KEYWORD_DECLARE,
			TOKEN_KEYWORD_CLASS,
			TOKEN_KEYWORD_OVERRIDE,
			TOKEN_KEYWORD_FINAL,
			TOKEN_KEYWORD_EXTEND,
			TOKEN_KEYWORD_ABSTRACT,
			TOKEN_KEYWORD_PUBLIC,
			TOKEN_KEYWORD_PROTECTED,
			TOKEN_KEYWORD_PRIVATE,
			TOKEN_KEYWORD_IF,
			TOKEN_KEYWORD_WHILE,
			TOKEN_KEYWORD_FOR,
			TOKEN_KEYWORD_FOREACH,
			TOKEN_KEYWORD_SWITCH,
			TOKEN_KEYWORD_CASE,
			TOKEN_KEYWORD_BREAK,
			TOKEN_KEYWORD_RETURN,
			TOKEN_KEYWORD_CONTINUE,

			TOKEN_DATATYPE_NONE,
			TOKEN_DATATYPE_VOID,
			TOKEN_DATATYPE_INTEGER,
			TOKEN_DATATYPE_FLOAT,
			TOKEN_DATATYPE_DOUBLE,
			TOKEN_DATATYPE_DECIMAL,
			TOKEN_DATATYPE_STRING,
			TOKEN_DATATYPE_CHAR,
			TOKEN_DATATYPE_UINTEGER,
			TOKEN_DATATYPE_BIGINTEGER,
			TOKEN_DATATYPE_BOOL,
			TOKEN_DATATYPE_INT8,
			TOKEN_DATATYPE_INT16,
			TOKEN_DATATYPE_INT32,
			TOKEN_DATATYPE_INT64,
			TOKEN_DATATYPE_UINT8,
			TOKEN_DATATYPE_UINT16,
			TOKEN_DATATYPE_UINT32,
			TOKEN_DATATYPE_UINT64,
			TOKEN_DATATYPE_LIST,
			TOKEN_DATATYPE_DICT,
			TOKEN_DATATYPE_ARRAY,

			TOKEN_IDENTIFIER,

			TOKEN_LITERAL_INTEGER,	// No suffix, Integer/Int32
			TOKEN_LITERAL_UNSIGNED,	// u, Uint32
			TOKEN_LITERAL_LONG,		// l, Int64
			TOKEN_LITERAL_SHORT,	// s, Int16
			TOKEN_LITERAL_DOUBLE,	// No suffix, Double
			TOKEN_LITERAL_FLOAT,	// f, Float
			TOKEN_LITERAL_DECIMAL,	// d, Decimal
			TOKEN_LITERAL_INT_BIN,	// b, Integer/Int32 binary
			TOKEN_LITERAL_INT_OCT,	// o, Integer/Int32 octal
			TOKEN_LITERAL_INT_HEX,	// h, Integer/Int32 hex
			TOKEN_LITERAL_STRING,
			TOKEN_LITERAL_CHAR,
			TOKEN_LITERAL_BOOL,
			TOKEN_LITERAL_NONE,

			TOKEN_OPERATOR_ASSIGNMENT,
			TOKEN_OPERATOR_BINARY,
			TOKEN_OPERATOR_UNARY,
			TOKEN_OPERATOR_CONTROL,

			TOKEN_EOF = 255
		};

		class Token
		{
		public:
			std::string file;
			unsigned long line;

			TokenType type;
			std::string value;

			Token(TokenType t = TOKEN_DEFAULT, const std::string &val = "") : file("(undefined)"), line(0), type(t), value(val) {}

			std::string typeToString() const;

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
