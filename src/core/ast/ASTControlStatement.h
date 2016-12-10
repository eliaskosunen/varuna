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
#include "core/ast/ASTStatement.h"
#include "core/ast/ASTExpression.h"

namespace core
{
	namespace ast
	{
		class ASTIfStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> condition;
			std::unique_ptr<ASTStatement> ifBlock, elseBlock;

			ASTIfStatement(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> ifb, std::unique_ptr<ASTStatement> elseb = nullptr)
				: condition(std::move(cond)), ifBlock(std::move(ifb)), elseBlock(std::move(elseb)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTForStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> init, rangeDecl, rangeInit;
			std::unique_ptr<ASTStatement> block;

			ASTForStatement(std::unique_ptr<ASTStatement> stmt, std::unique_ptr<ASTExpression> _init = nullptr, std::unique_ptr<ASTExpression> _rangeDecl = nullptr, std::unique_ptr<ASTExpression> _rangeInit = nullptr)
				: init(std::move(_init)), rangeDecl(std::move(_rangeDecl)), rangeInit(std::move(_rangeInit)), block(std::move(stmt)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTForeachStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> iteratee, iterator;
			std::unique_ptr<ASTBlockStatement> block;

			ASTForeachStatement(std::unique_ptr<ASTExpression> _iteratee, std::unique_ptr<ASTExpression> _iterator, std::unique_ptr<ASTBlockStatement> _block)
				: iteratee(std::move(_iteratee)), iterator(std::move(_iterator)), block(std::move(_block)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTWhileStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> condition;
			std::unique_ptr<ASTBlockStatement> block;

			ASTWhileStatement(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTBlockStatement> _block)
				: condition(std::move(cond)), block(std::move(_block)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTImportStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> importee;
			bool isPath;

			enum ImportType
			{
				UNSPECIFIED = 0,
				MODULE,
				PACKAGE
			} importType;

			ASTImportStatement(ImportType type, std::unique_ptr<ASTIdentifierExpression> toImport, bool _isPath = false)
				: importee(std::move(toImport)), isPath(_isPath), importType(type) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTModuleStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> moduleName;

			ASTModuleStatement(std::unique_ptr<ASTIdentifierExpression> name)
				: moduleName(std::move(name)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};
	}
}
