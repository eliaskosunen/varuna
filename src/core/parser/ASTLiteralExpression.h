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
#include <boost/variant.hpp>

namespace core
{
	namespace parser
	{
		class ASTLiteralExpression : public ASTExpression
		{
		public:
			virtual ~ASTLiteralExpression() {}
		};

		class ASTIntegerLiteralExpression : public ASTLiteralExpression
		{
			boost::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t> value;

			core::lexer::TokenIntegerLiteralModifier mod;
		public:
			ASTIntegerLiteralExpression(int8_t val) : value(val) {}
			ASTIntegerLiteralExpression(int16_t val) : value(val) {}
			ASTIntegerLiteralExpression(int32_t val) : value(val) {}
			ASTIntegerLiteralExpression(int64_t val) : value(val) {}
			ASTIntegerLiteralExpression(uint8_t val) : value(val) {}
			ASTIntegerLiteralExpression(uint16_t val) : value(val) {}
			ASTIntegerLiteralExpression(uint32_t val) : value(val) {}
			ASTIntegerLiteralExpression(uint64_t val) : value(val) {}
		};

		class ASTFloatLiteralExpression : public ASTLiteralExpression
		{
			boost::variant<float, double> value;

			core::lexer::TokenFloatLiteralModifier mod;
		public:
			ASTFloatLiteralExpression(float val) : value(val) {}
			ASTFloatLiteralExpression(double val) : value(val) {}
		};

		class ASTStringLiteralExpression : public ASTLiteralExpression
		{
			const std::string &value;
		public:
			ASTStringLiteralExpression(const std::string &val) : value(val) {}
		};

		class ASTCharLiteralExpression : public ASTLiteralExpression
		{
			char32_t value;
		public:
			ASTCharLiteralExpression(char32_t val) : value(val) {}
		};

		class ASTBoolLiteralExpression : public ASTLiteralExpression
		{
			bool value;
		public:
			ASTBoolLiteralExpression(bool val) : value(val) {}
		};

		class ASTNoneLiteralExpression : public ASTLiteralExpression
		{
		public:
			ASTNoneLiteralExpression() {}
		};
	}
}
