/*
Copyright (C) 2016-2017 Elias Kosunen

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

#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"

namespace core
{
	namespace ast
	{
		class ASTFunctionParameter : public ASTStatement
		{
		public:
			std::unique_ptr<ASTVariableDefinitionExpression> var;
			enum PassType
			{
				COPY = 0,
				REF = 1,
				VIEW = 2
			} passType;

			ASTFunctionParameter(std::unique_ptr<ASTVariableDefinitionExpression> _var, PassType _passType = COPY)
				: ASTStatement(FUNCTION_PARAMETER), var(std::move(_var)), passType(_passType) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
		};

		class ASTFunctionPrototypeStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> name, returnType;
			std::vector<std::unique_ptr<ASTFunctionParameter>> params;

			ASTFunctionPrototypeStatement(std::unique_ptr<ASTIdentifierExpression> _name, std::unique_ptr<ASTIdentifierExpression> retType, std::vector<std::unique_ptr<ASTFunctionParameter>> _params)
				: ASTStatement(FUNCTION_PROTO_STMT), name(std::move(_name)), returnType(std::move(retType)), params(std::move(_params)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTFunctionDefinitionStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTFunctionPrototypeStatement> proto;
			std::unique_ptr<ASTBlockStatement> body;

			ASTFunctionDefinitionStatement(std::unique_ptr<ASTFunctionPrototypeStatement> _proto, std::unique_ptr<ASTBlockStatement> _body)
				: ASTStatement(FUNCTION_DEF_STMT), proto(std::move(_proto)), body(std::move(_body)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTFunctionDeclarationStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTFunctionPrototypeStatement> proto;
			bool isExtern;

			ASTFunctionDeclarationStatement(std::unique_ptr<ASTFunctionPrototypeStatement> _proto, bool _isExtern = false)
				: ASTStatement(FUNCTION_DECL_STMT), proto(std::move(_proto)), isExtern(_isExtern) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTReturnStatement : public ASTStatement
		{
		public:
			std::unique_ptr<ASTExpression> returnValue;

			ASTReturnStatement(std::unique_ptr<ASTExpression> retval = nullptr)
				: ASTStatement(RETURN_STMT), returnValue(std::move(retval)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};
	} // namespace ast
} // namespace core
