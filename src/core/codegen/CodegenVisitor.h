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

#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTNode.h"
#include "core/ast/AST.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/Visitor.h"
#include "util/Logger.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

#include <utility>

#define USE_LLVM_FUNCTION_VERIFY 0
#define USE_LLVM_MODULE_VERIFY 0

namespace core
{
	namespace codegen
	{
		struct Variable
		{
			llvm::Value *value;
			llvm::Type *type;
			const std::string &name;
		};

		class CodegenVisitor : public ast::Visitor
		{
			llvm::LLVMContext &context;
			llvm::IRBuilder<> builder;
			llvm::Module *module;
			std::unordered_map<std::string, Variable> variables;
			std::unordered_map<std::string, Variable> globalVariables;
			std::unordered_map<std::string, std::unique_ptr<ast::ASTFunctionPrototypeStatement>> functionProtos;
			std::unordered_map<std::string, llvm::Type*> types;
		public:
			CodegenVisitor(llvm::LLVMContext &c, llvm::Module *m);

			bool codegen(ast::AST *ast);

			llvm::Value *getDummyValue() const
			{
				static llvm::Constant *ret = llvm::Constant::getNullValue(findType("int32"));
				return ret;
			}

			template <typename... Args>
			std::nullptr_t codegenError(const std::string &format, Args... args) const
			{
				util::logger->error(format.c_str(), args...);
				return nullptr;
			}

			template <typename... Args>
			void codegenWarning(const std::string &format, Args... args) const
			{
				util::logger->warn(format.c_str(), args...);
			}

			llvm::Type *findType(const std::string &name) const
			{
				auto type = types.find(name);
				if(type == types.end())
				{
					return codegenError("Undefined typename: '{}'", name);
				}
				return type->second;
			}

			llvm::Function *findFunction(const std::string &name)
			{
				llvm::Function *f = module->getFunction(name);
				if(f)
				{
					return f;
				}

				auto fit = functionProtos.find(name);
				if(fit != functionProtos.end())
				{
					return fit->second->accept(this);
				}

				return codegenError("Undefined function: '{}'", name);
			}

			void dumpModule() const
			{
				module->dump();
			}

			llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *func, const Variable &var)
			{
				llvm::IRBuilder<> tmp(&func->getEntryBlock(), func->getEntryBlock().begin());
				return tmp.CreateAlloca(var.type, nullptr, var.name.c_str());
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
			llvm::LoadInst *visit(ast::ASTVariableRefExpression *expr);
			llvm::Value *visit(ast::ASTCallExpression *expr);
			llvm::Value *visit(ast::ASTCastExpression *node);
			llvm::Value *visit(ast::ASTVariableDefinitionExpression *expr);

			llvm::Value *visit(ast::ASTFunctionParameter *node);
			llvm::Function *visit(ast::ASTFunctionPrototypeStatement *stmt);
			llvm::Function *visit(ast::ASTFunctionDefinitionStatement *stmt);
			llvm::Value *visit(ast::ASTFunctionDeclarationStatement *node);
			llvm::Value *visit(ast::ASTReturnStatement *stmt);

			llvm::Constant *visit(ast::ASTIntegerLiteralExpression *expr);
			llvm::Constant *visit(ast::ASTFloatLiteralExpression *expr);
			llvm::Constant *visit(ast::ASTStringLiteralExpression *expr);
			llvm::Constant *visit(ast::ASTCharLiteralExpression *expr);
			llvm::Constant *visit(ast::ASTBoolLiteralExpression *expr);
			llvm::Constant *visit(ast::ASTNoneLiteralExpression *node);

			llvm::Value *visit(ast::ASTBinaryOperationExpression *expr);
			llvm::Value *visit(ast::ASTUnaryOperationExpression *expr);
			llvm::Value *visit(ast::ASTAssignmentOperationExpression *node);

			llvm::Value *visit(ast::ASTEmptyStatement *node);
			llvm::Value *visit(ast::ASTBlockStatement *stmt);
			llvm::Value *visit(ast::ASTWrappedExpressionStatement *stmt);
		};
	} // namespace codegen
} // namespace core
