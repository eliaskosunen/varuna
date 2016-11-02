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

#include "core/parser/StatementBase.h"
#include "core/parser/Operator.h"
#include "core/parser/ExpressionBase.h"

namespace core
{
	namespace parser
	{
		class StatementAssignment: public Statement
		{
			std::shared_ptr<AssignmentOperator> oper;
			std::shared_ptr<Expression> lvalue;
			std::shared_ptr<Expression> rvalue;
		};

		class StatementIf: public Statement
		{
			std::shared_ptr<Expression> condition;
			std::shared_ptr<Statement> ifBranch;
			std::shared_ptr<Statement> elseBranch;
		};

		class StatementFor: public StatementBlock
		{

		};

		class StatementWhile: public StatementBlock
		{

		};

		class StatementVariableDeclaration: public Statement
		{

		};

		class StatementFunctionDeclaration: public Statement
		{

		};

		class StatementFunctionDefinition: public StatementBlock
		{

		};

		class StatementFunctionCallVoid: public Statement
		{

		};
	}
}
