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

#include "util/SafeEnum.h"

namespace core
{
	namespace ast
	{
		class ASTNode
		{
		public:
			enum NodeType_t
			{
				NODE = -1,

				EXPR = 0,
				ASSIGNMENT_OPERATION_EXPR,
				BINARY_OPERATION_EXPR,
				BOOL_LITERAL_EXPR,
				CALL_EXPR,
				CAST_EXPR = 5,
				CHAR_LITERAL_EXPR,
				EMPTY_EXPR,
				FLOAT_LITERAL_EXPR,
				IDENTIFIER_EXPR,
				VARIABLE_REF_EXPR = 10,
				INTEGER_LITERAL_EXPR,
				NONE_LITERAL_EXPR,
				STRING_LITERAL_EXPR,
				UNARY_OPERATION_EXPR,
				VARIABLE_DEFINITION_EXPR = 15,

				STMT = 100,
				BLOCK_STMT,
				EMPTY_STMT,
				FOREACH_STMT,
				FOR_STMT,
				FUNCTION_DECL_STMT = 105,
				FUNCTION_DEF_STMT,
				FUNCTION_PARAMETER,
				FUNCTION_PROTO_STMT,
				IF_STMT,
				IMPORT_STMT = 110,
				MODULE_STMT,
				RETURN_STMT,
				WHILE_STMT,
				WRAPPED_EXPR_STMT
			};

			using NodeType = util::SafeEnum<NodeType_t>;
			NodeType nodeType {NODE};

			ASTNode *parent {nullptr};

			virtual void accept(DumpASTVisitor *v, size_t ind = 0) = 0;
			virtual void accept(ASTParentSolverVisitor *v, ASTNode *p) = 0;

			ASTNode() = default;
			ASTNode(const ASTNode&) = default;
			ASTNode &operator = (const ASTNode&) = default;
			ASTNode(ASTNode&&) = default;
			ASTNode &operator = (ASTNode&&) = default;
			virtual ~ASTNode() = default;
		protected:
			ASTNode(NodeType t) : nodeType(t) {}
		};
	} // namespace ast
} // namespace core
