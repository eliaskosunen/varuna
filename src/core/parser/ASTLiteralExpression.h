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

#include "core/parser/FwdDecl.h"
#include "core/parser/ASTExpression.h"

#include "core/lexer/Token.h"
#include "util/SafeEnum.h"

#include <string>

namespace core
{
	namespace parser
	{
		class ASTLiteralExpression : public ASTExpression
		{
			enum LiteralType
			{
				INTEGER,
				FLOAT,
				STRING,
				CHAR,
				BOOL,
				NONE
			} literalType;
			union
			{
				long long valInt;
				unsigned long long valIntU;
				long double valFloat;
				std::string valStr;
				unsigned char valChar;
				bool valBool;
				int valNone;
			};

			core::lexer::TokenIntegerLiteralModifier modifierInt;
			core::lexer::TokenFloatLiteralModifier modifierFloat;
		public:
			ASTLiteralExpression()
				: literalType(NONE), valNone(0), modifierInt(core::lexer::INTEGER_INTEGER), modifierFloat(core::lexer::FLOAT_FLOAT) {}
			ASTLiteralExpression(long long val, const core::lexer::TokenIntegerLiteralModifier &mod = core::lexer::INTEGER_INTEGER)
				: literalType(INTEGER), valInt(val), modifierInt(mod), modifierFloat(core::lexer::FLOAT_FLOAT) {}
			ASTLiteralExpression(unsigned long long val, const core::lexer::TokenIntegerLiteralModifier &mod = core::lexer::INTEGER_INTEGER)
				: literalType(INTEGER), valIntU(val), modifierInt(mod), modifierFloat(core::lexer::FLOAT_FLOAT)
			{
				modifierInt |= core::lexer::INTEGER_UNSIGNED;
			}
			ASTLiteralExpression(long double val, const core::lexer::TokenFloatLiteralModifier &mod = core::lexer::FLOAT_FLOAT)
				: literalType(FLOAT), valFloat(val), modifierInt(core::lexer::INTEGER_INTEGER), modifierFloat(mod) {}
			explicit ASTLiteralExpression(const std::string &val)
				: literalType(STRING), valStr(val), modifierInt(core::lexer::INTEGER_INTEGER),
				modifierFloat(core::lexer::FLOAT_FLOAT) {}
			explicit ASTLiteralExpression(unsigned char val)
				: literalType(CHAR), valChar(val), modifierInt(core::lexer::INTEGER_INTEGER), modifierFloat(core::lexer::FLOAT_FLOAT) {}
			explicit ASTLiteralExpression(bool val)
				: literalType(BOOL), valBool(val), modifierInt(core::lexer::INTEGER_INTEGER),
				modifierFloat(core::lexer::FLOAT_FLOAT) {}

			~ASTLiteralExpression() {}

			template <typename T>
			T getValue()
			{
				switch(literalType)
				{
				case INTEGER:
				{
					if(modifierInt.isSet(core::lexer::INTEGER_UNSIGNED))
					{
						return static_cast<T>(valIntU);
					}
					return static_cast<T>(valInt);
				}
				case FLOAT:
					return static_cast<T>(valFloat);
				case STRING:
					return static_cast<T>(valStr);
				case CHAR:
					return static_cast<T>(valChar);
				case BOOL:
					return static_cast<T>(valBool != 0);
				case NONE:
					return static_cast<T>(valNone);
				}
			}
		};
	}
}
