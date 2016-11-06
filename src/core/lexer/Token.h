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
		enum TokenType : short
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
			TOKEN_KEYWORD_IMPLEMENT,
			TOKEN_KEYWORD_INTERFACE,
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
			TOKEN_LITERAL_TRUE,
			TOKEN_LITERAL_FALSE,
			TOKEN_LITERAL_NONE,

			TOKEN_OPERATORA_SIMPLE,	// =
			TOKEN_OPERATORA_ADD,	// +=
			TOKEN_OPERATORA_SUB,	// -=
			TOKEN_OPERATORA_MUL,	// *=
			TOKEN_OPERATORA_DIV,	// /=
			TOKEN_OPERATORA_MOD,	// %=

			TOKEN_OPERATORA_BITAND,	// &=
			TOKEN_OPERATORA_BITOR,	// |=
			TOKEN_OPERATORA_BITXOR,	// ^=
			TOKEN_OPERATORA_SHIFTL,	// <<=
			TOKEN_OPERATORA_SHIFTR,	// >>=

			TOKEN_OPERATORB_ADD,	// a + b
			TOKEN_OPERATORB_SUB,	// a - b
			TOKEN_OPERATORB_MUL,	// a * b
			TOKEN_OPERATORB_DIV,	// a / b
			TOKEN_OPERATORB_MOD,	// a % b

			TOKEN_OPERATORB_BITAND,	// a & b
			TOKEN_OPERATORB_BITOR,	// a | b
			TOKEN_OPERATORB_BITXOR,	// a ^ b
			TOKEN_OPERATORB_SHIFTL,	// a << b
			TOKEN_OPERATORB_SHIFTR,	// a >> b

			TOKEN_OPERATORB_AND,	// a && b, a and b
			TOKEN_OPERATORB_OR,		// a || b, a or b

			TOKEN_OPERATORB_EQ,		// a == b
			TOKEN_OPERATORB_NOTEQ,	// a != b
			TOKEN_OPERATORB_LESS,	// a < b
			TOKEN_OPERATORB_GREATER,// a > b
			TOKEN_OPERATORB_LESSEQ,	// a <= b
			TOKEN_OPERATORB_GREATEQ,// a >= b

			TOKEN_OPERATORB_MEMBER,	// a.b
			TOKEN_OPERATORB_ACCESS,	// a[b]

			TOKEN_OPERATORB_OF,		// List of String
			TOKEN_OPERATORB_AS,		// foreach(list as String elem)

			TOKEN_OPERATORU_INCPRE,	// ++a
			TOKEN_OPERATORU_DECPRE,	// --a
			TOKEN_OPERATORU_INCPOST,// a++
			TOKEN_OPERATORU_DECPOST,// a--

			TOKEN_OPERATORU_PLUS,	// +a
			TOKEN_OPERATORU_MINUS,	// -a
			TOKEN_OPERATORU_BITNOT,	// ~a
			TOKEN_OPERATORU_NOT,	// !a, not a

			TOKEN_OPERATORU_CAST,	// (Integer)a
			TOKEN_OPERATORU_SIZEOF,	// sizeof(a)
			TOKEN_OPERATORU_TYPEOF,	// typeof(a)
			TOKEN_OPERATORU_NEW,	// new A

			TOKEN_PUNCT_PAREN_OPEN,	// (
			TOKEN_PUNCT_PAREN_CLOSE,// )
			TOKEN_PUNCT_BRACE_OPEN,	// {
			TOKEN_PUNCT_BRACE_CLOSE,// }
			TOKEN_PUNCT_COLON,		// :
			TOKEN_PUNCT_SEMICOLON,	// ;

			TOKEN_EOF = 32767
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
