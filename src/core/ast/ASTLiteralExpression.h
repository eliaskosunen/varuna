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

#include "core/ast/FwdDecl.h"
#include "core/ast/ASTExpression.h"

#include "core/lexer/Token.h"
#include "util/SafeEnum.h"

#include <string>
#include <boost/variant.hpp>

namespace core
{
	namespace ast
	{
		class ASTIntegerLiteralExpression : public ASTExpression
		{
		public:
			boost::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t> value;

			core::lexer::TokenIntegerLiteralModifier mod;

			ASTIntegerLiteralExpression(core::lexer::TokenIntegerLiteralModifier _mod = core::lexer::INTEGER_INTEGER) : value(0), mod(_mod) {}
			ASTIntegerLiteralExpression(int8_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			ASTIntegerLiteralExpression(int16_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			ASTIntegerLiteralExpression(int32_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			ASTIntegerLiteralExpression(int64_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			ASTIntegerLiteralExpression(uint8_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			ASTIntegerLiteralExpression(uint16_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			ASTIntegerLiteralExpression(uint32_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}
			ASTIntegerLiteralExpression(uint64_t val, core::lexer::TokenIntegerLiteralModifier _mod) : value(val), mod(_mod) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};

		class ASTFloatLiteralExpression : public ASTExpression
		{
		public:
			boost::variant<float, double> value;

			core::lexer::TokenFloatLiteralModifier mod;

			ASTFloatLiteralExpression(core::lexer::TokenFloatLiteralModifier _mod = core::lexer::FLOAT_FLOAT) : value(0.0), mod(_mod) {}
			ASTFloatLiteralExpression(float val, core::lexer::TokenFloatLiteralModifier _mod) : value(val), mod(_mod) {}
			ASTFloatLiteralExpression(double val, core::lexer::TokenFloatLiteralModifier _mod) : value(val), mod(_mod) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};

		class ASTStringLiteralExpression : public ASTExpression
		{
		public:
			const std::string &value;

			ASTStringLiteralExpression(const std::string &val) : value(val) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};

		class ASTCharLiteralExpression : public ASTExpression
		{
		public:
			char32_t value;

			ASTCharLiteralExpression(char32_t val) : value(val) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};

		class ASTBoolLiteralExpression : public ASTExpression
		{
		public:
			bool value;

			ASTBoolLiteralExpression(bool val) : value(val) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};

		class ASTNoneLiteralExpression : public ASTExpression
		{
		public:
			ASTNoneLiteralExpression() {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};
	}
}
