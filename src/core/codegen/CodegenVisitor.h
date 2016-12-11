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
#include "core/ast/Visitor.h"
#include "core/ast/ASTFunctionStatement.h"
#include "util/Logger.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace core
{
	namespace codegen
	{
		struct Type
		{
			llvm::Type *underlying;
			bool isReducedClass{false};
		};

		struct Variable
		{
			llvm::Value *value;
			Type type;
		};

		class CodegenVisitor : public ast::Visitor
		{
			llvm::LLVMContext context;
			llvm::IRBuilder<> builder;
			std::unique_ptr<llvm::Module> module;
			std::unordered_map<std::string, Variable> variables;
			std::unordered_map<std::string, Variable> globalVariables;
			std::unordered_map<std::string, std::unique_ptr<ast::ASTFunctionPrototypeStatement>> functionProtos;
			std::unordered_map<std::string, Type> types;
		public:
			CodegenVisitor();

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

			template <typename... Args>
			std::nullptr_t codegenError(const std::string &format, Args... args)
			{
				util::logger->error(format.c_str(), args...);
				return nullptr;
			}

			template <typename... Args>
			void codegenWarning(const std::string &format, Args... args)
			{
				util::logger->warn(format.c_str(), args...);
			}

			llvm::Type *findType(const std::string &name)
			{
				auto type = types.find(name);
				if(type == types.end())
				{
					return codegenError("Undefined typename: '{}'", name);
				}
				return type->second.underlying;
			}

			llvm::Value *visit(ast::ASTNode *node) = delete;
			llvm::Value *visit(ast::ASTStatement *stmt);
			llvm::Value *visit(ast::ASTExpression *expr);

			llvm::Value *visit(ast::ASTIfStatement *node);
			llvm::Value *visit(ast::ASTForStatement *node);
			llvm::Value *visit(ast::ASTForeachStatement *node);
			llvm::Value *visit(ast::ASTWhileStatement *node);
			llvm::Value *visit(ast::ASTImportStatement *node);
			llvm::Value *visit(ast::ASTModuleStatement *node);

			llvm::Value *visit(ast::ASTEmptyExpression *node);
			llvm::Value *visit(ast::ASTIdentifierExpression *node);
			llvm::LoadInst *visit(ast::ASTVariableRefExpression *node);
			llvm::Value *visit(ast::ASTCallExpression *node);
			llvm::Value *visit(ast::ASTCastExpression *node);
			llvm::Value *visit(ast::ASTVariableDefinitionExpression *node);

			llvm::Value *visit(ast::ASTFunctionParameter *node);
			llvm::Function *visit(ast::ASTFunctionPrototypeStatement *node);
			llvm::Function *visit(ast::ASTFunctionDefinitionStatement *node);
			llvm::Value *visit(ast::ASTFunctionDeclarationStatement *node);
			llvm::Value *visit(ast::ASTReturnStatement *node);

			llvm::ConstantInt *visit(ast::ASTIntegerLiteralExpression *node);
			llvm::Constant *visit(ast::ASTFloatLiteralExpression *node);
			llvm::Constant *visit(ast::ASTStringLiteralExpression *node);
			llvm::ConstantInt *visit(ast::ASTCharLiteralExpression *node);
			llvm::ConstantInt *visit(ast::ASTBoolLiteralExpression *node);
			llvm::Constant *visit(ast::ASTNoneLiteralExpression *node);

			llvm::Value *visit(ast::ASTBinaryOperationExpression *node);
			llvm::Value *visit(ast::ASTUnaryOperationExpression *node);
			llvm::Value *visit(ast::ASTAssignmentOperationExpression *node);

			llvm::Value *visit(ast::ASTEmptyStatement *node);
			llvm::Value *visit(ast::ASTBlockStatement *node);
			llvm::Value *visit(ast::ASTWrappedExpressionStatement *node);
		};
	}
}
