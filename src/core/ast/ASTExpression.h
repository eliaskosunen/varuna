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

			ASTExpression() {}
			virtual ~ASTExpression() {}
		};

		class ASTIdentifierExpression : public ASTExpression
		{
		public:
			std::string value;

			ASTIdentifierExpression(std::string val) : value(std::move(val)) {}

			virtual void accept(DumpASTVisitor *v, size_t ind = 0);

			virtual ~ASTIdentifierExpression() {}
		};

		class ASTVariableRefExpression : public ASTIdentifierExpression
		{
		public:
			ASTVariableRefExpression(std::string val) : ASTIdentifierExpression(std::move(val)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};

		class ASTCallExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTExpression> callee;
			std::vector<std::unique_ptr<ASTExpression>> params;

			ASTCallExpression(std::unique_ptr<ASTExpression> _callee, std::vector<std::unique_ptr<ASTExpression>> _params)
				: callee(std::move(_callee)), params(std::move(_params)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};

		class ASTCastExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> type, castee;

			ASTCastExpression(std::unique_ptr<ASTIdentifierExpression> _castee, std::unique_ptr<ASTIdentifierExpression> _type)
				: type(std::move(_type)), castee(std::move(_castee)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};

		class ASTVariableDefinitionExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> typen, name;
			enum Type
			{
				INTEGER,
				INT8, INT16, INT32, INT64,
				UINT8, UINT16, UINT32, UINT64,

				FLOAT,
				F32, F64,

				STRING, CHAR,
				BOOL, NONE,

				UDEF
			} type;
			std::unique_ptr<ASTExpression> init;

			ASTVariableDefinitionExpression(Type t, std::unique_ptr<ASTIdentifierExpression> _type, std::unique_ptr<ASTIdentifierExpression> _name, std::unique_ptr<ASTExpression> _init = nullptr)
				: typen(std::move(_type)), name(std::move(_name)), type(t), init(std::move(_init)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0);
		};
	}
}
