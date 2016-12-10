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
#include "core/ast/ASTNode.h"

#include <memory>
#include <vector>

namespace core
{
	namespace ast
	{
		class ASTExpression : public ASTNode
		{
		public:
			virtual void accept(DumpASTVisitor *v, size_t ind = 0);
			virtual llvm::Value *accept(codegen::CodegenVisitor *v);

			ASTExpression() = default;
			ASTExpression(const ASTExpression&) = default;
			ASTExpression &operator = (const ASTExpression&) = default;
			ASTExpression(ASTExpression&&) = default;
			ASTExpression &operator = (ASTExpression&&) = default;
			virtual ~ASTExpression() = default;
		};

		class ASTEmptyExpression : public ASTExpression
		{
		public:
			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTIdentifierExpression : public ASTExpression
		{
		public:
			std::string value {""};

			ASTIdentifierExpression() = default;
			ASTIdentifierExpression(std::string val) : value(val) {}
			ASTIdentifierExpression(const ASTIdentifierExpression&) = default;
			ASTIdentifierExpression &operator = (const ASTIdentifierExpression&) = default;
			ASTIdentifierExpression(ASTIdentifierExpression&&) = default;
			ASTIdentifierExpression &operator = (ASTIdentifierExpression&&) = default;
			virtual ~ASTIdentifierExpression() = default;

			virtual void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTVariableRefExpression : public ASTIdentifierExpression
		{
		public:
			ASTVariableRefExpression(std::string val) : ASTIdentifierExpression(val) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTCallExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTExpression> callee;
			std::vector<std::unique_ptr<ASTExpression>> params;

			ASTCallExpression(std::unique_ptr<ASTExpression> _callee, std::vector<std::unique_ptr<ASTExpression>> _params)
				: callee(std::move(_callee)), params(std::move(_params)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTCastExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> type, castee;

			ASTCastExpression(std::unique_ptr<ASTIdentifierExpression> _castee, std::unique_ptr<ASTIdentifierExpression> _type)
				: type(std::move(_type)), castee(std::move(_castee)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};

		class ASTVariableDefinitionExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> typen, name;
			enum Type
			{
				INTEGER,
				INT8, INT16, INT32, INT64,

				FLOAT,
				F32, F64,

				STRING, CHAR,
				BOOL, NONE,

				UDEF
			} type;
			std::unique_ptr<ASTExpression> init;
			uint32_t arraySize;

			ASTVariableDefinitionExpression(Type t, std::unique_ptr<ASTIdentifierExpression> _type, std::unique_ptr<ASTIdentifierExpression> _name, std::unique_ptr<ASTExpression> _init = nullptr, uint32_t arrSize = 0)
				: typen(std::move(_type)), name(std::move(_name)), type(t), init(std::move(_init)), arraySize(arrSize) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
			llvm::Value *accept(codegen::CodegenVisitor *v);
		};
	}
}
