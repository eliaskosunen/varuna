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

#include <memory>

#include "core/parser/ExpressionBase.h"
#include "core/parser/Operator.h"

namespace core
{
	namespace parser
	{
		class ExpressionBinaryOperation: public Expression
		{
			std::shared_ptr<BinaryOperator> oper;
			std::shared_ptr<Expression> left;
			std::shared_ptr<Expression> right;
		};
		class ExpressionUnaryOperation: public Expression
		{

		};
		class ExpressionFunctionCall: public Expression
		{

		};

		class ExpressionLiteral: public Expression
		{

		};
		class ExpressionIdentifier: public Expression
		{

		};

		class ExpressionLiteralInteger: public ExpressionLiteral
		{

		};
		class ExpressionLiteralFloat: public ExpressionLiteral
		{

		};
		class ExpressionLiteralString: public ExpressionLiteral
		{

		};
		class ExpressionLiteralBoolean: public ExpressionLiteral
		{

		};
		class ExpressionLiteralNone: public ExpressionLiteral
		{

		};
	}
}
