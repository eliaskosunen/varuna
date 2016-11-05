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
#include <string>
#include <vector>

#include "core/parser/ExpressionBase.h"
#include "core/parser/Operator.h"

namespace core
{
	namespace parser
	{
		class ExpressionBinaryOperation: public Expression
		{
			std::unique_ptr<BinaryOperator> oper;
			std::unique_ptr<Expression> left;
			std::unique_ptr<Expression> right;
		public:
			ExpressionBinaryOperation() : Expression() {}

			BinaryOperator *getOperator() { return oper.get(); }
			Expression *getLeft() { return left.get(); }
			Expression *getRight() { return right.get(); }
		};
		class ExpressionUnaryOperation: public Expression
		{
			std::unique_ptr<UnaryOperator> oper;
			std::unique_ptr<Expression> right;
		public:
			ExpressionUnaryOperation() : Expression() {}

			UnaryOperator *getOperator() { return oper.get(); }
			Expression *getRight() { return right.get(); }
		};
		class ExpressionFunctionCall: public Expression
		{
			std::unique_ptr<ExpressionIdentifier> name;
			//std::vector<std::unique_ptr<StatementFunctionParameter>> paramList;
		public:
			ExpressionFunctionCall() : Expression() {}

			ExpressionIdentifier *getName() { return name.get(); }
		};

		class ExpressionLiteral: public Expression
		{
		public:
			ExpressionLiteral() : Expression() {}
		};
		class ExpressionIdentifier: public Expression
		{
			std::string name;
		public:
			ExpressionIdentifier(std::string _name) : Expression(), name(_name) {}

			const std::string &getName() const { return name; }
			void setName(const std::string &name_) { name = name_; }
		};

		class ExpressionLiteralInteger: public ExpressionLiteral
		{
			long long value;
		public:
			ExpressionLiteralInteger(long long val) : ExpressionLiteral(), value(val) {}

			long long getValue() const { return value; }
			void setValue(long long val) { value = val; }
		};
		class ExpressionLiteralFloat: public ExpressionLiteral
		{
			long double value;
		public:
			ExpressionLiteralFloat(long double val) : ExpressionLiteral(), value(val) {}

			long double getValue() const { return value; }
			void setValue(long double val) { value = val; }
		};
		class ExpressionLiteralString: public ExpressionLiteral
		{
			std::string value;
		public:
			ExpressionLiteralString(const std::string &val) : ExpressionLiteral(), value(val) {}

			const std::string &getValue() const { return value; }
			void setValue(const std::string &val) { value = val; }
		};
		class ExpressionLiteralBoolean: public ExpressionLiteral
		{
			bool value;
		public:
			ExpressionLiteralBoolean(bool val) : ExpressionLiteral(), value(val) {}

			bool getValue() const { return value; }
			void setValue(bool val) { value = val; }
		};
		class ExpressionLiteralNone: public ExpressionLiteral
		{
		public:
			ExpressionLiteralNone() : ExpressionLiteral() {}
		};
	}
}
