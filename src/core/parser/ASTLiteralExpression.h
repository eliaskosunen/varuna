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
		class ASTIntegerLiteralExpression : public ASTExpression
		{
		public:
			boost::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t> value;

			core::lexer::TokenIntegerLiteralModifier mod;

			explicit ASTIntegerLiteralExpression(core::lexer::TokenIntegerLiteralModifier _mod = core::lexer::INTEGER_INTEGER) : value(0), mod(_mod) {}
			explicit ASTIntegerLiteralExpression(int8_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			explicit ASTIntegerLiteralExpression(int16_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			explicit ASTIntegerLiteralExpression(int32_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			explicit ASTIntegerLiteralExpression(int64_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			explicit ASTIntegerLiteralExpression(uint8_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			explicit ASTIntegerLiteralExpression(uint16_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			explicit ASTIntegerLiteralExpression(uint32_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			explicit ASTIntegerLiteralExpression(uint64_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
		};

		class ASTFloatLiteralExpression : public ASTExpression
		{
		public:
			boost::variant<float, double> value;

			core::lexer::TokenFloatLiteralModifier mod;

			explicit ASTFloatLiteralExpression(core::lexer::TokenFloatLiteralModifier _mod = core::lexer::FLOAT_FLOAT) : value(0.0), mod(_mod) {}
			explicit ASTFloatLiteralExpression(float val, core::lexer::TokenFloatLiteralModifier _mod) : value(val), mod(_mod) {}
			explicit ASTFloatLiteralExpression(double val, core::lexer::TokenFloatLiteralModifier _mod) : value(val), mod(_mod) {}
		};

		class ASTStringLiteralExpression : public ASTExpression
		{
		public:
			const std::string &value;

			explicit ASTStringLiteralExpression(const std::string &val) : value(val) {}
		};

		class ASTCharLiteralExpression : public ASTExpression
		{
		public:
			char32_t value;

			explicit ASTCharLiteralExpression(char32_t val) : value(val) {}
		};

		class ASTBoolLiteralExpression : public ASTExpression
		{
		public:
			bool value;

			explicit ASTBoolLiteralExpression(bool val) : value(val) {}
		};

		class ASTNoneLiteralExpression : public ASTExpression
		{
		public:
			ASTNoneLiteralExpression() {}
		};
	}
}
