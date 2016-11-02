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
			std::shared_ptr<Expression> init;
			std::shared_ptr<Expression> check;
			std::shared_ptr<Expression> increment;
			// statementList is inherited
		};

		class StatementWhile: public StatementBlock
		{
			std::shared_ptr<Expression> condition;
			// statementList is inherited
		};

		class StatementVariableDeclaration: public Statement
		{
			// std::shared_ptr<DataType> type
			std::shared_ptr<ExpressionIdentifier> name;
			std::shared_ptr<Expression> value;
			// bool isConst;
			// bool isExtern;
			// bool isExport;
		};

		class StatementFunctionDeclaration: public Statement
		{
			// std::shared_ptr<DataType> returnType;
			std::shared_ptr<ExpressionIdentifier> name;
			// std::vector<std::shared_ptr<StatementFunctionParameter>> paramList;
			// bool isExtern;
			// bool isExport;
		};

		class StatementFunctionDefinition: public StatementBlock
		{
			// std::shared_ptr<DataType> returnType;
			std::shared_ptr<ExpressionIdentifier> name;
			// std::vector<std::shared_ptr<StatementFunctionParameter>> paramList;
			// bool isExport;
			// statementList is inherited
		};

		class StatementFunctionCallVoid: public Statement
		{
			std::shared_ptr<ExpressionIdentifier> name;
			// std::vector<std::shared_ptr<StatementFunctionParameter>> paramList;
		};
	}
}
