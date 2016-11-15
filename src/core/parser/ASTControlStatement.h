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

namespace core
{
	namespace parser
	{
		class ASTIfStatement : public ASTStatement
		{
			std::unique_ptr<ASTExpression> condition;
			std::unique_ptr<ASTBlockStatement> ifBlock, elseBlock;
		public:
			ASTIfStatement(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTBlockStatement> ifb, std::unique_ptr<ASTBlockStatement> elseb = nullptr)
				: condition(std::move(cond)), ifBlock(std::move(ifb)), elseBlock(std::move(elseb)) {}
		};

		class ASTForStatement : public ASTStatement
		{
			std::unique_ptr<ASTExpression> init, rangeDecl, rangeInit;
			std::unique_ptr<ASTBlockStatement> block;
		public:
			explicit ASTForStatement(std::unique_ptr<ASTBlockStatement> stmt, std::unique_ptr<ASTExpression> _init = nullptr, std::unique_ptr<ASTExpression> _rangeDecl = nullptr, std::unique_ptr<ASTExpression> _rangeInit = nullptr)
				: init(std::move(_init)), rangeDecl(std::move(_rangeDecl)), rangeInit(std::move(_rangeInit)), block(std::move(stmt)) {}
		};

		class ASTForeachStatement : public ASTStatement
		{
			std::unique_ptr<ASTExpression> iteratee, iterator;
			std::unique_ptr<ASTBlockStatement> block;
		public:
			ASTForeachStatement(std::unique_ptr<ASTExpression> _iteratee, std::unique_ptr<ASTExpression> _iterator, std::unique_ptr<ASTBlockStatement> _block)
				: iteratee(std::move(_iteratee)), iterator(std::move(_iterator)), block(std::move(_block)) {}
		};

		class ASTWhileStatement : public ASTStatement
		{
			std::unique_ptr<ASTExpression> condition;
			std::unique_ptr<ASTBlockStatement> block;
		public:
			explicit ASTWhileStatement(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTBlockStatement> _block = nullptr)
				: condition(std::move(cond)), block(std::move(_block)) {}
		};

		class ASTImportStatement : public ASTStatement
		{
			std::unique_ptr<Identifier> importee;
		public:
			enum ImportType
			{
				UNSPECIFIED = 0,
				MODULE,
				PACKAGE
			} importType;

			ASTImportStatement(ImportType type, std::unique_ptr<Identifier> toImport)
				: importee(std::move(toImport)), importType(type) {}
		};
	}
}
