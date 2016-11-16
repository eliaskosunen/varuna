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
#include "core/parser/ASTStatement.h"
#include "core/parser/ASTExpression.h"

namespace core
{
	namespace parser
	{
		class ASTFunctionDeclarationStatement : public ASTStatement
		{
		public:
			std::unique_ptr<Identifier> returnType, name;
			std::vector<std::unique_ptr<ASTVariableDefinitionStatement>> params;

			ASTFunctionDeclarationStatement(std::unique_ptr<Identifier> retType, std::unique_ptr<Identifier> n, std::vector<std::unique_ptr<ASTVariableDefinitionStatement>> p)
				: returnType(std::move(retType)), name(std::move(n)), params(std::move(p)) {}
		};

		class ASTFunctionDefinitionStatement : public ASTFunctionDeclarationStatement
		{
		public:
			std::unique_ptr<ASTBlockStatement> block;

			ASTFunctionDefinitionStatement(std::unique_ptr<Identifier> retType, std::unique_ptr<Identifier> n, std::unique_ptr<ASTBlockStatement> b,  std::vector<std::unique_ptr<ASTVariableDefinitionStatement>> p)
				: ASTFunctionDeclarationStatement(std::move(retType), std::move(n), std::move(p)), block(std::move(b)) {}
		};

		class ASTReturnStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> returnValue;

			explicit ASTReturnStatement(std::unique_ptr<ASTExpression> retval = nullptr)
				: returnValue(std::move(retval)) {}
		};
	}
}
