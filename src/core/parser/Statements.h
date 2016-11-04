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
			std::unique_ptr<AssignmentOperator> oper;
			std::unique_ptr<Expression> lvalue;
			std::unique_ptr<Expression> rvalue;
		public:
			StatementAssignment() : Statement() {}

			AssignmentOperator *getOperator() { return oper.get(); }
			Expression *getLValue() { return lvalue.get(); }
			Expression *getRValue() { return rvalue.get(); }
		};

		class StatementIf: public Statement
		{
			std::unique_ptr<Expression> condition;
			std::unique_ptr<Statement> ifBranch;
			std::unique_ptr<Statement> elseBranch;
			bool usingElse;
		public:
			StatementIf() : Statement(), usingElse(false) {}

			Expression *getCondition() { return condition.get(); }
			Statement *getIfBranch() { return ifBranch.get(); }
			Statement *getElseBranch() { return elseBranch.get(); }

			bool getUsingElse() const { return usingElse; }
			void setUsingElse(bool u) { usingElse = u; }
		};

		class StatementFor: public StatementBlock
		{
			std::unique_ptr<Expression> init;		// Integer i = 0
			std::unique_ptr<Expression> condition;	// i < 10
			std::unique_ptr<Expression> iteration;	// ++i
			// statementList is inherited
		public:
			StatementFor() : StatementBlock() {}

			Expression *getInit() { return init.get(); }
			Expression *getCondition() { return condition.get(); }
			Expression *getIteration() { return iteration.get(); }
		};

		class StatementWhile: public StatementBlock
		{
			std::unique_ptr<Expression> condition;
			// statementList is inherited
		public:
			StatementWhile() : StatementBlock() {}

			Expression *getCondition() { return condition.get(); }
		};

		class StatementVariableDeclaration: public Statement
		{
			// std::unique_ptr<DataType> type
			std::unique_ptr<ExpressionIdentifier> name;
			std::unique_ptr<Expression> value;
			bool isConst;
			bool isExtern;
			bool isExport;
		public:
			StatementVariableDeclaration();

			// DataType *getDataType() { return type.get(); }
			ExpressionIdentifier *getName() { return name.get(); }
			Expression *getValue() { return value.get(); }

			bool getIsConst() const { return isConst; }
			void setIsConst(bool c) { isConst = c; }

			bool getIsExtern() const { return isExtern; }
			void setIsExtern(bool e) { isExtern = e; }

			bool getIsExport() const { return isExport; }
			void setIsExport(bool e) { isExport = e; }
		};

		class StatementFunctionDeclaration: public Statement
		{
			// std::unique_ptr<DataType> returnType;
			std::unique_ptr<ExpressionIdentifier> name;
			// std::vector<std::unique_ptr<StatementFunctionParameter>> paramList;
			bool isExtern;
			bool isExport;
		public:
			StatementFunctionDeclaration();

			ExpressionIdentifier *getName() { return name.get(); }

			bool getIsExtern() const { return isExtern; }
			void setIsExtern(bool e) { isExtern = e; }

			bool getIsExport() const { return isExport; }
			void setIsExport(bool e) { isExport = e; }
		};

		class StatementFunctionDefinition: public StatementBlock
		{
			// std::unique_ptr<DataType> returnType;
			std::unique_ptr<ExpressionIdentifier> name;
			// std::vector<std::unique_ptr<StatementFunctionParameter>> paramList;
			bool isExport;
			// statementList is inherited
		public:
			StatementFunctionDefinition();

			ExpressionIdentifier *getName() { return name.get(); }

			bool getIsExport() const { return isExport; }
			void setIsExport(bool e) { isExport = e; }
		};

		class StatementFunctionCallVoid: public Statement
		{
			std::unique_ptr<ExpressionIdentifier> name;
			// std::vector<std::unique_ptr<StatementFunctionParameter>> paramList;
		public:
			StatementFunctionCallVoid();

			ExpressionIdentifier *getName() { return name.get(); }
		};

		class StatementImport: public Statement
		{
			std::unique_ptr<ExpressionIdentifier> from;
			std::unique_ptr<ExpressionIdentifier> symbolToImport;
			std::unique_ptr<ExpressionIdentifier> importAs;
			bool isImportAll;
			bool isFrom;
			bool isAs;
		public:
			StatementImport();

			ExpressionIdentifier *getFrom() { return from.get(); }
			ExpressionIdentifier *getSymbol() { return symbolToImport.get(); }
			ExpressionIdentifier *getAs() { return importAs.get(); }

			bool getIsImportAll() const { return isImportAll; }
			void setIsImportAll(bool ia) { isImportAll = ia; }

			bool getIsFrom() const { return isFrom; }
			void setIsFrom(bool f) { isFrom = f; }

			bool getIsAs() const { return isAs; }
			void setIsAs(bool a) { isAs = a; }

			ExpressionIdentifier *getModuleToImport();
			ExpressionIdentifier *getActualSymbol();
			ExpressionIdentifier *getSymbolNameToUse();
		};
	}
}
