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

#include "core/lexer/Lexer.h"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "util/Logger.h"
#include "util/StringUtils.h"

namespace core
{
	namespace lexer
	{
		Token Lexer::getNextToken(std::string::const_iterator &it)
		{
			const std::string::const_iterator end = content.end();
			char currentChar = *it;

			util::loggerBasic->trace("");

			if(it == end)
			{
				util::logger->trace("Current character is EOF");
				return createToken(TOKEN_EOF, "EOF");
			}

			util::logger->trace("Getting next token. Current character: '{}', on line {}", currentChar, currentLine);

			// Skip any whitespace
			while(util::StringUtils::isCharWhitespace(currentChar))
			{
				// Increase currentLine if current character is a newline character
				if(currentChar == '\n')
				{
					++currentLine;
				}

				util::logger->trace("Skipping whitespace");
				currentChar = *(++it);

				if(it == end)
				{
					util::logger->trace("Current character is EOF");
					return createToken(TOKEN_EOF, "EOF");
				}
			}

			if(util::StringUtils::isCharControlCharacter(currentChar) && !util::StringUtils::isCharWhitespace(currentChar))
			{
				fmt::MemoryWriter out;
				out << "'" << currentChar
					<< "', dec: " << util::StringUtils::charToString(currentChar)
					<< " hex: " << fmt::hex(currentChar);
				lexerWarning("Unrecognized character: {}", out.str());
				++it;
				return createToken(TOKEN_DEFAULT, util::StringUtils::charToString(currentChar));
			}

			// Word: a keyword or an identifier
			// [a-zA-Z_][a-zA-Z_0-9]*
			if(util::StringUtils::isCharAlpha(currentChar) || currentChar == '_')
			{
				std::string buf;
				buf.reserve(8);
				buf.push_back(currentChar);
				while(util::StringUtils::isCharAlnum(*(++it)) || *it == '_')
				{
					buf.push_back(*it);
				}
				util::logger->trace("Word: '{}'", buf);

				// Identify the meaning of the word
				return getTokenFromWord(buf);
			}

			// Number literal
			// -?[0-9]([0-9\.]*)[dfulsboh]
			if(
				util::StringUtils::isCharDigit(currentChar) ||
				(currentChar == '-' && util::StringUtils::isCharDigit(*(it + 1))) ||
				(currentChar == '.' && util::StringUtils::isCharDigit(*(it + 1)))
			)
			{
				std::string buf;
				buf.reserve(8);
				bool isFloatingPoint = (currentChar == '.');
				bool isHex = false;
				if(currentChar == '0' && *(it + 1) == 'x')
				{
					currentChar = *(it += 2);
					isHex = true;
				}
				bool cont = true;
				do
				{
					buf.push_back(currentChar);
					currentChar = *(++it);
					if(currentChar == '.')
					{
						isFloatingPoint = true;
					}
					cont = (isHex ? util::StringUtils::isCharHexDigit(currentChar) : util::StringUtils::isCharDigit(currentChar));
				} while(cont || currentChar == '.');
				util::logger->trace("Number literal: '{}'", buf);

				if(!isFloatingPoint)
				{
					Token t = createToken(TOKEN_LITERAL_INTEGER, buf);
					while(util::StringUtils::isCharAlpha(currentChar))
					{
						switch(currentChar)
						{
						case 'u':
							t.modifierInt |= INTEGER_UNSIGNED;
							break;
						case 'l':
							t.modifierInt |= INTEGER_LONG;
							break;
						case 's':
							t.modifierInt |= INTEGER_SHORT;
							break;
						case 'b':
							t.modifierInt |= INTEGER_BINARY;
							break;
						case 'o':
							t.modifierInt |= INTEGER_OCTAL;
							break;
						default:
							lexerError("Invalid integer literal suffix: '{}'", currentChar);
						}
						currentChar = *(++it);
					}
					if(isHex)
					{
						t.modifierInt |= INTEGER_HEX;
					}
					return t;
				}
				else
				{
					Token t = createToken(TOKEN_LITERAL_FLOAT, buf);
					while(util::StringUtils::isCharAlpha(currentChar))
					{
						switch(currentChar)
						{
						case 'f':
							t.modifierFloat |= FLOAT_FLOAT;
							break;
						case 'd':
							t.modifierFloat |= FLOAT_DECIMAL;
							break;
						default:
							lexerError("Invalid float literal suffix: '{}'", currentChar);
						}
						currentChar = *(++it);
					}
					if(isHex)
					{
						lexerError("Floating-point number cannot be in hexadecimal");
					}
					return t;
				}
			}

			// String literal
			// ".+"
			if(currentChar == '"')
			{
				std::string buf = lexStringLiteral(it, end);
				if(!getError())
				{
					util::logger->trace("String literal: '{}'", buf);

					return createToken(TOKEN_LITERAL_STRING, buf);
				}

			}

			// Character literal
			// '.'
			if(currentChar == '\'')
			{
				std::string buf = lexStringLiteral(it, end, true);
				if(!getError())
				{
					util::logger->trace("Character literal: '{}'", buf);

					return createToken(TOKEN_LITERAL_CHAR, buf);
				}
			}

			// Operator or punctuator
			if(util::StringUtils::isCharPunctuation(currentChar))
			{
				std::string buf;
				while(util::StringUtils::isCharPunctuation(currentChar))
				{
					if(getTokenTypeFromOperator(buf) != TOKEN_UNDEFINED)
					{
						std::string tmp(buf);
						tmp.push_back(*it);
						if(getTokenTypeFromOperator(tmp) == TOKEN_UNDEFINED)
						{
							break;
						}
					}
					buf.push_back(currentChar);
					currentChar = *(++it);
				}
				Token t = getTokenFromOperator(buf);
				if(t.type == TOKEN_UNDEFINED)
				{
					lexerError("Invalid operator or punctuator: '{}'", buf);
				}
				util::logger->trace("Operator/punctuator: '{}'", buf);
				return t;
			}

			fmt::MemoryWriter out;
			out << "'" << currentChar
				<< "', dec: " << util::StringUtils::charToString(currentChar)
				<< " hex: " << fmt::hex(currentChar);
			lexerWarning("Unrecognized token: {}", out.str());
			++it;
			return createToken(TOKEN_DEFAULT, util::StringUtils::charToString(currentChar));
		}

		TokenVector Lexer::run()
		{
			// Initialize variables
			TokenVector tokens;

			std::string::const_iterator strpointer = content.begin();

			while(true)
			{
				Token t = getNextToken(strpointer);
				if(getError())
				{
					break;
				}
				tokens.push_back(t);
				util::logger->trace("Pushed new token: ({}): '{}'", t.typeToString(), t.value);
				if(t.type == TOKEN_EOF)
				{
					util::logger->trace("Token is EOF, stop");
					break;
				}
			}

			if(tokens.size() == 0)
			{
				lexerError("Empty token list");
			}
			if(tokens.size() == 1 && tokens[0].type == core::lexer::TOKEN_EOF)
			{
				lexerWarning("Empty translation unit");
			}
			syntaxCheck(tokens);
			return tokens;
		}

		void Lexer::syntaxCheck(const TokenVector &tokens)
		{
			unsigned int parenL = 0, parenR = 0, bracketL = 0, bracketR = 0, braceL = 0, braceR = 0;
			for(const auto &t : tokens)
			{
				if(t.type == TOKEN_PUNCT_PAREN_OPEN) ++parenL;
				if(t.type == TOKEN_PUNCT_PAREN_CLOSE) ++parenR;
				if(t.type == TOKEN_PUNCT_SQR_OPEN) ++bracketL;
				if(t.type == TOKEN_PUNCT_SQR_CLOSE) ++bracketR;
				if(t.type == TOKEN_PUNCT_BRACE_OPEN) ++braceL;
				if(t.type == TOKEN_PUNCT_BRACE_CLOSE) ++braceR;
			}
			if(parenL != parenR)
			{
				lexerError("Syntax Error: Parenthesis mismatch");
			}
			if(bracketL != bracketR)
			{
				lexerError("Syntax Error: Bracket mismatch");
			}
			if(braceL != braceR)
			{
				lexerError("Syntax Error: Brace mismatch");
			}
		}

		std::string Lexer::lexStringLiteral(std::string::const_iterator &it, const std::string::const_iterator &end, bool isChar)
		{
			std::string buf;
			buf.reserve(isChar ? 2 : 8);
			const char quote = (isChar ? '\'' : '"');
			while((++it) != end)
			{
				char currentChar = *it;
				char prev = *(it - 1);
				char next = *(it + 1);
				util::logger->trace("Current character: '{}', prev: '{}', next: '{}'", currentChar, prev, next);

				// Current char is a newline
				// String doesn't end, terminate
				if(currentChar == '\n')
				{
					lexerError("Invalid {} literal: Closing quote not found", (isChar ? "character" : "string"));
					break;
				}
				// Current char is a quotation mark
				if(currentChar == quote)
				{
					if(prev == '\\' && *(it - 2) != '\\')
					{
						// Remove the backslash
						buf.pop_back();
					}
					// Not escaped, end string
					else
					{
						util::logger->trace("Current character is an unescaped quotation mark, end");
						break;
					}
				}

				buf.push_back(currentChar);
				util::logger->trace("Pushed buffer: '{}'", buf);
			}

			util::logger->trace("Buffer before escaping: '{}'", buf);
			std::string escaped;
			std::string::const_iterator rit = buf.begin();
			while(rit != buf.end())
			{
				char c = *rit++;
				if(c == '\\' && rit != buf.end())
				{
					switch(*rit++)
					{
					case '\\':
						c = '\\';
						break;
					case 'n':
						c = '\n';
						break;
					case 't':
						c = '\t';
						break;
					case 'r':
						c = '\r';
						break;
					case 'f':
						c = '\f';
						break;
					case 'v':
						c = '\v';
						break;
					case 'b':
						c = '\b';
						break;
					case 'a':
						c = '\a';
						break;
					case 'x':
					{
						std::string hex;
						if(!util::StringUtils::isCharHexDigit(*rit))
						{
							lexerWarning("Invalid hexadecimal escape sequence: The first character after an \\x should be a number in hexadecimal, got '{}' instead", *rit);
							continue;
						}
						hex.push_back(*rit);
						if(util::StringUtils::isCharHexDigit(*(rit + 1)))
						{
							++rit;
							hex.push_back(*rit);
						}
						++rit;
						std::istringstream hexcharss(hex);
						int ch;
						hexcharss >> std::hex >> ch;
						c = ch;
						break;
					}
					case 'o':
					{
						std::string oct;
						if(!util::StringUtils::isCharOctDigit(*rit))
						{
							lexerWarning("Invalid octal escape sequence: The first character after an \\o should be a number in octal, got '{}' instead", *rit);
							continue;
						}
						oct.push_back(*rit);
						if(util::StringUtils::isCharOctDigit(*(rit + 1)))
						{
							++rit;
							oct.push_back(*rit);
							if(util::StringUtils::isCharOctDigit(*(rit + 1)))
							{
								++rit;
								oct.push_back(*rit);
							}
							++rit;
						}
						else
						{
							++rit;
						}
						std::istringstream octcharss(oct);
						int ch;
						octcharss >> std::oct >> ch;
						c = ch;
						break;
					}
					case '"':
						if(!isChar)
						{
							c = '"';
						}
						break;
					case '\'':
						if(isChar)
						{
							c = '\'';
						}
						break;
					default:
						lexerWarning("Invalid escape sequence");
						continue;
					}
				}
				escaped.push_back(c);
			}

			if(isChar && escaped.length() > 1)
			{
				lexerError("Invalid character literal: Length more than 1: '{}'", escaped);
			}
			++it;
			return escaped;
		}

		Token Lexer::createToken(TokenType type, const std::string &val) const
		{
			return Token::create(type, val, currentLine, filename);
		}

		TokenType Lexer::getTokenTypeFromWord(const std::string &buf) const
		{
			// Keywords
			if(buf == "import")		return TOKEN_KEYWORD_IMPORT;
			if(buf == "function")	return TOKEN_KEYWORD_FUNCTION;
			if(buf == "declare")	return TOKEN_KEYWORD_DECLARE;
			if(buf == "class")		return TOKEN_KEYWORD_CLASS;
			if(buf == "override")	return TOKEN_KEYWORD_OVERRIDE;
			if(buf == "final")		return TOKEN_KEYWORD_FINAL;
			if(buf == "extend")		return TOKEN_KEYWORD_EXTEND;
			if(buf == "abstract")	return TOKEN_KEYWORD_ABSTRACT;
			if(buf == "implement")	return TOKEN_KEYWORD_IMPLEMENT;
			if(buf == "interface")	return TOKEN_KEYWORD_INTERFACE;
			if(buf == "public")		return TOKEN_KEYWORD_PUBLIC;
			if(buf == "protected")	return TOKEN_KEYWORD_PROTECTED;
			if(buf == "private")	return TOKEN_KEYWORD_PRIVATE;
			if(buf == "if")			return TOKEN_KEYWORD_IF;
			if(buf == "else")		return TOKEN_KEYWORD_ELSE;
			if(buf == "while")		return TOKEN_KEYWORD_WHILE;
			if(buf == "for")		return TOKEN_KEYWORD_FOR;
			if(buf == "foreach")	return TOKEN_KEYWORD_FOREACH;
			if(buf == "switch")		return TOKEN_KEYWORD_SWITCH;
			if(buf == "case")		return TOKEN_KEYWORD_CASE;
			if(buf == "break")		return TOKEN_KEYWORD_BREAK;
			if(buf == "return")		return TOKEN_KEYWORD_RETURN;
			if(buf == "continue")	return TOKEN_KEYWORD_CONTINUE;

			// Datatypes
			if(buf == "None")		return TOKEN_DATATYPE_NONE;
			if(buf == "Void")		return TOKEN_DATATYPE_VOID;
			if(buf == "Integer")	return TOKEN_DATATYPE_INTEGER;
			if(buf == "Float")		return TOKEN_DATATYPE_FLOAT;
			if(buf == "Double")		return TOKEN_DATATYPE_DOUBLE;
			if(buf == "Decimal")	return TOKEN_DATATYPE_DECIMAL;
			if(buf == "String")		return TOKEN_DATATYPE_STRING;
			if(buf == "Char")		return TOKEN_DATATYPE_CHAR;
			if(buf == "UInteger")	return TOKEN_DATATYPE_UINTEGER;
			if(buf == "BigInteger")	return TOKEN_DATATYPE_BIGINTEGER;
			if(buf == "Bool")		return TOKEN_DATATYPE_BOOL;
			if(buf == "Int8")		return TOKEN_DATATYPE_INT8;
			if(buf == "Int16")		return TOKEN_DATATYPE_INT16;
			if(buf == "Int32")		return TOKEN_DATATYPE_INT32;
			if(buf == "Int64")		return TOKEN_DATATYPE_INT64;
			if(buf == "UInt8")		return TOKEN_DATATYPE_UINT8;
			if(buf == "UInt16")		return TOKEN_DATATYPE_UINT16;
			if(buf == "UInt32")		return TOKEN_DATATYPE_UINT32;
			if(buf == "UInt64")		return TOKEN_DATATYPE_UINT64;
			if(buf == "List")		return TOKEN_DATATYPE_LIST;
			if(buf == "Dict")		return TOKEN_DATATYPE_DICT;
			if(buf == "Array")		return TOKEN_DATATYPE_ARRAY;

			// Literals
			if(buf == "true")		return TOKEN_LITERAL_TRUE;
			if(buf == "false")		return TOKEN_LITERAL_FALSE;
			if(buf == "none")		return TOKEN_LITERAL_NONE;

			// Textual operators
			if(buf == "and")		return TOKEN_OPERATORB_AND;
			if(buf == "or")			return TOKEN_OPERATORB_OR;
			if(buf == "not")		return TOKEN_OPERATORU_NOT;
			if(buf == "of")			return TOKEN_OPERATORB_OF;
			if(buf == "as")			return TOKEN_OPERATORB_AS;
			if(buf == "sizeof")		return TOKEN_OPERATORU_SIZEOF;
			if(buf == "typeof")		return TOKEN_OPERATORU_TYPEOF;
			if(buf == "new")		return TOKEN_OPERATORU_NEW;

			return TOKEN_IDENTIFIER;
		}

		Token Lexer::getTokenFromWord(const std::string &buf) const
		{
			return createToken(getTokenTypeFromWord(buf), buf);
		}

		TokenType Lexer::getTokenTypeFromOperator(const std::string &buf) const
		{
			if(buf == "=")			return TOKEN_OPERATORA_SIMPLE;
			if(buf == "+=")			return TOKEN_OPERATORA_ADD;
			if(buf == "-=")			return TOKEN_OPERATORA_SUB;
			if(buf == "*=")			return TOKEN_OPERATORA_MUL;
			if(buf == "/=")			return TOKEN_OPERATORA_DIV;
			if(buf == "%=")			return TOKEN_OPERATORA_MOD;

			if(buf == "&=")			return TOKEN_OPERATORA_BITAND;
			if(buf == "|=")			return TOKEN_OPERATORA_BITOR;
			if(buf == "^=")			return TOKEN_OPERATORA_BITXOR;
			if(buf == "<<=")		return TOKEN_OPERATORA_SHIFTL;
			if(buf == ">>=")		return TOKEN_OPERATORA_SHIFTR;

			if(buf == "+")			return TOKEN_OPERATORB_ADD;
			if(buf == "-")			return TOKEN_OPERATORB_SUB;
			if(buf == "*")			return TOKEN_OPERATORB_MUL;
			if(buf == "/")			return TOKEN_OPERATORB_DIV;
			if(buf == "%")			return TOKEN_OPERATORB_MOD;

			if(buf == "&")			return TOKEN_OPERATORB_BITAND;
			if(buf == "|")			return TOKEN_OPERATORB_BITOR;
			if(buf == "^")			return TOKEN_OPERATORB_BITXOR;
			if(buf == "<<")			return TOKEN_OPERATORB_SHIFTL;
			if(buf == ">>")			return TOKEN_OPERATORB_SHIFTR;

			if(buf == "&&")			return TOKEN_OPERATORB_AND;
			if(buf == "||")			return TOKEN_OPERATORB_OR;

			if(buf == "==")			return TOKEN_OPERATORB_EQ;
			if(buf == "!=")			return TOKEN_OPERATORB_NOTEQ;
			if(buf == "<")			return TOKEN_OPERATORB_LESS;
			if(buf == ">")			return TOKEN_OPERATORB_GREATER;
			if(buf == "<=")			return TOKEN_OPERATORB_LESSEQ;
			if(buf == ">=")			return TOKEN_OPERATORB_GREATEQ;

			if(buf == ".")			return TOKEN_OPERATORB_MEMBER;

			if(buf == "++")			return TOKEN_OPERATORU_INC;
			if(buf == "--")			return TOKEN_OPERATORU_DEC;

			if(buf == "~")			return TOKEN_OPERATORU_BITNOT;
			if(buf == "!")			return TOKEN_OPERATORU_NOT;

			if(buf == "(")			return TOKEN_PUNCT_PAREN_OPEN;
			if(buf == ")")			return TOKEN_PUNCT_PAREN_CLOSE;
			if(buf == "{")			return TOKEN_PUNCT_BRACE_OPEN;
			if(buf == "}")			return TOKEN_PUNCT_BRACE_CLOSE;
			if(buf == "[")			return TOKEN_PUNCT_SQR_OPEN;
			if(buf == "]")			return TOKEN_PUNCT_SQR_CLOSE;
			if(buf == ":")			return TOKEN_PUNCT_COLON;
			if(buf == ";")			return TOKEN_PUNCT_SEMICOLON;
			if(buf == ",")			return TOKEN_PUNCT_COMMA;

			return TOKEN_UNDEFINED;
		}

		Token Lexer::getTokenFromOperator(const std::string &buf) const
		{
			return createToken(getTokenTypeFromOperator(buf), buf);
		}
	}
}
