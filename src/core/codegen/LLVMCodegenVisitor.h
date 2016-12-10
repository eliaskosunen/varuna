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

#include "core/codegen/CodegenVisitorBase.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/ASTNode.h"
#include "util/Logger.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace core
{
	namespace codegen
	{
		class LLVMCodegenVisitor : public CodegenVisitorBase
		{
			llvm::LLVMContext context;
			llvm::IRBuilder<> builder;
			std::unique_ptr<llvm::Module> module;
			std::map<std::string, llvm::Value*> namedValues;
		public:
			LLVMCodegenVisitor();

			template <typename T>
			void codegen(T *root)
			{
				auto casted = dynamic_cast<ast::ASTNode*>(root);
				if(!casted)
				{
					throw std::invalid_argument("Invalid root node given to code generator");
				}
				visit(root);
			}

			void visit(ast::ASTNode *node) = delete;
			void visit(ast::ASTStatement *stmt) = delete;
			void visit(ast::ASTExpression *expr) = delete;

			void visit(ast::ASTIfStatement *node);
			void visit(ast::ASTForStatement *node);
			void visit(ast::ASTForeachStatement *node);
			void visit(ast::ASTWhileStatement *node);
			void visit(ast::ASTImportStatement *node);
			void visit(ast::ASTModuleStatement *node);

			void visit(ast::ASTEmptyExpression *node);
			void visit(ast::ASTIdentifierExpression *node);
			void visit(ast::ASTVariableRefExpression *node);
			void visit(ast::ASTCallExpression *node);
			void visit(ast::ASTCastExpression *node);
			void visit(ast::ASTVariableDefinitionExpression *node);

			void visit(ast::ASTFunctionParameter *node);
			void visit(ast::ASTFunctionPrototypeStatement *node);
			void visit(ast::ASTFunctionDefinitionStatement *node);
			void visit(ast::ASTFunctionDeclarationStatement *node);
			void visit(ast::ASTReturnStatement *node);

			void visit(ast::ASTIntegerLiteralExpression *node);
			void visit(ast::ASTFloatLiteralExpression *node);
			void visit(ast::ASTStringLiteralExpression *node);
			void visit(ast::ASTCharLiteralExpression *node);
			void visit(ast::ASTBoolLiteralExpression *node);
			void visit(ast::ASTNoneLiteralExpression *node);

			void visit(ast::ASTBinaryOperationExpression *node);
			void visit(ast::ASTUnaryOperationExpression *node);
			void visit(ast::ASTAssignmentOperationExpression *node);

			void visit(ast::ASTEmptyStatement *node);
			void visit(ast::ASTBlockStatement *node) {}
			void visit(ast::ASTWrappedExpressionStatement *node);
		};
	}
}
