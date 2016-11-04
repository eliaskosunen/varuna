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

#include "core/parser/Statements.h"

#include <memory>

#include "core/parser/StatementBase.h"
#include "core/parser/Operator.h"
#include "core/parser/ExpressionBase.h"
#include "core/parser/Expressions.h"
#include "util/Compatibility.h"

namespace core
{
	namespace parser
	{
		StatementVariableDeclaration::StatementVariableDeclaration() : Statement(), isConst(false), isExtern(false), isExport(false)
		{
			name->setName("(undefined)");
		}

		StatementFunctionDeclaration::StatementFunctionDeclaration() : Statement(), isExtern(false), isExport(false)
		{
			name->setName("(undefined)");
		}

		StatementFunctionDefinition::StatementFunctionDefinition() : StatementBlock(), isExport(false)
		{
			name->setName("(undefined)");
		}

		StatementFunctionCallVoid::StatementFunctionCallVoid() : Statement()
		{
			name->setName("(undefined)");
		}

		StatementImport::StatementImport() : Statement(), isImportAll(true), isFrom(false), isAs(false) {}

		ExpressionIdentifier *StatementImport::getModuleToImport()
		{
			if(isFrom)
			{
				return getFrom();
			}
			return getSymbol();
		}

		ExpressionIdentifier *StatementImport::getActualSymbol()
		{
			return getSymbol();
		}

		ExpressionIdentifier *StatementImport::getSymbolNameToUse()
		{
			if(isAs)
			{
				return getAs();
			}
			return getSymbol();
		}
	}
}
