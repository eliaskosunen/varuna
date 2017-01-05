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

#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"

#include <memory>
#include <vector>

namespace core
{
	namespace ast
	{
		class ASTExpression : public ASTNode
		{
		protected:
			ASTExpression(NodeType t) : ASTNode(t) {}

		public:
			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			virtual std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v);
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;

			ASTExpression() : ASTNode(EXPR) {}
			ASTExpression(const ASTExpression&) = delete;
			ASTExpression &operator = (const ASTExpression&) = delete;
			ASTExpression(ASTExpression&&) = default;
			ASTExpression &operator = (ASTExpression&&) = default;
			~ASTExpression() override = default;
		};

		class ASTEmptyExpression : public ASTExpression
		{
		public:
			ASTEmptyExpression() : ASTExpression(EMPTY_EXPR) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTIdentifierExpression : public ASTExpression
		{
		protected:
			ASTIdentifierExpression(NodeType t, std::string val) : ASTExpression(t), value(std::move(val)) {}

		public:
			std::string value;

			ASTIdentifierExpression(std::string val) : ASTExpression(IDENTIFIER_EXPR), value(std::move(val)) {}
			ASTIdentifierExpression(const ASTIdentifierExpression&) = delete;
			ASTIdentifierExpression &operator = (const ASTIdentifierExpression&) = delete;
			ASTIdentifierExpression(ASTIdentifierExpression&&) = default;
			ASTIdentifierExpression &operator = (ASTIdentifierExpression&&) = default;
			~ASTIdentifierExpression() override = default;

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTVariableRefExpression : public ASTIdentifierExpression
		{
		public:
			ASTVariableRefExpression(std::string val)
				: ASTIdentifierExpression(VARIABLE_REF_EXPR, val) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<core::codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTCallExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTExpression> callee;
			std::vector<std::unique_ptr<ASTExpression>> params;

			ASTCallExpression(std::unique_ptr<ASTExpression> _callee, std::vector<std::unique_ptr<ASTExpression>> _params)
				: ASTExpression(CALL_EXPR), callee(std::move(_callee)), params(std::move(_params)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTCastExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> type;
			std::unique_ptr<ASTExpression> castee;

			ASTCastExpression(std::unique_ptr<ASTExpression> _castee, std::unique_ptr<ASTIdentifierExpression> _type)
				: ASTExpression(CAST_EXPR), type(std::move(_type)), castee(std::move(_castee)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTVariableDefinitionExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTIdentifierExpression> type, name;
			std::unique_ptr<ASTExpression> init;
			uint32_t arraySize;

			ASTVariableDefinitionExpression(std::unique_ptr<ASTIdentifierExpression> _type, std::unique_ptr<ASTIdentifierExpression> _name, std::unique_ptr<ASTExpression> _init = nullptr, uint32_t arrSize = 0)
				: ASTExpression(VARIABLE_DEFINITION_EXPR), type(std::move(_type)), name(std::move(_name)),
					init(std::move(_init)), arraySize(arrSize) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTSubscriptExpression : public ASTExpression
		{
		protected:
			ASTSubscriptExpression(NodeType t, std::unique_ptr<ASTExpression> lhs_, std::unique_ptr<ASTExpression> index_)
				: ASTExpression(t), lhs(std::move(lhs_)), index(std::move(index_)) {}

		public:
			std::unique_ptr<ASTExpression> lhs;
			std::unique_ptr<ASTExpression> index;

			ASTSubscriptExpression(std::unique_ptr<ASTExpression> lhs_, std::unique_ptr<ASTExpression> index_)
				: ASTExpression(SUBSCRIPT_EXPR), lhs(std::move(lhs_)), index(std::move(index_)) {}

			ASTSubscriptExpression(const ASTSubscriptExpression&) = delete;
			ASTSubscriptExpression(ASTSubscriptExpression&&) = default;
			ASTSubscriptExpression &operator =(const ASTSubscriptExpression&) = delete;
			ASTSubscriptExpression &operator =(ASTSubscriptExpression&&) = default;
			virtual ~ASTSubscriptExpression() = default;

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTSubscriptRangedExpression : public ASTSubscriptExpression
		{
		public:
			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};

		class ASTMemberAccessExpression : public ASTExpression
		{
		public:
			std::unique_ptr<ASTExpression> lhs, rhs;

			ASTMemberAccessExpression(std::unique_ptr<ASTExpression> l, std::unique_ptr<ASTExpression> r)
				: ASTExpression(MEMBER_ACCESS_EXPR), lhs(std::move(l)), rhs(std::move(r)) {}

			void accept(DumpASTVisitor *v, size_t ind = 0) override;
			std::unique_ptr<codegen::TypedValue> accept(codegen::CodegenVisitor *v) override;
			void accept(ASTParentSolverVisitor *v, ASTNode *p) override;
			bool accept(codegen::GrammarCheckerVisitor *v) override;
		};
	} // namespace ast
} // namespace core
