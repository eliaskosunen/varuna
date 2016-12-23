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
#include "core/codegen/CodegenInfo.h"
#include "util/Logger.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

#include <utility>

#define USE_LLVM_FUNCTION_VERIFY 0
#define USE_LLVM_MODULE_VERIFY 0

namespace core
{
	namespace codegen
	{
		struct Type
		{
			llvm::Type *type;
			llvm::DIType *dtype;
			std::string name;

			Type(llvm::Type *t, llvm::DIType *d, const std::string &n)
				: type(t), dtype(d), name(n) {}
		};

		struct Variable
		{
			llvm::Value *value;
			Type *type;
			const std::string &name;
		};

		class CodegenVisitor : public ast::Visitor
		{
			llvm::LLVMContext &context;
			llvm::Module *module;
			const CodegenInfo &info;
			llvm::IRBuilder<> builder;

			std::unique_ptr<llvm::DIBuilder> dbuilder;
			llvm::DICompileUnit *dcu;
			std::vector<llvm::DIScope*> dBlocks;

			std::unordered_map<std::string, Variable> variables;
			std::unordered_map<std::string, Variable> globalVariables;
			std::unordered_map<std::string, std::unique_ptr<ast::ASTFunctionPrototypeStatement>> functionProtos;
			std::unordered_map<std::string, std::unique_ptr<Type>> types;

			std::array<std::unique_ptr<Type>, 11> _buildTypeArray();
			std::unordered_map<std::string, std::unique_ptr<Type>> _createTypeMap();

			void emitDebugLocation(ast::ASTNode *node);

		public:
			CodegenVisitor(llvm::LLVMContext &c, llvm::Module *m, const CodegenInfo &i);

			bool codegen(ast::AST *ast);

			llvm::Value *getDummyValue() const
			{
				static llvm::Constant *ret = llvm::Constant::getNullValue(findType("int32")->type);
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

			Type *findType(const std::string &name) const
			{
				auto type = types.find(name);
				if(type == types.end())
				{
					return codegenError("Undefined typename: '{}'", name);
				}
				return type->second.get();
			}

			Type *findType(llvm::Type *type) const
			{
				for(const auto &t : types)
				{
					if(t.second->type == type)
					{
						return t.second.get();
					}
				}
				return nullptr;
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

			std::string createFunctionName(llvm::Value*)
			{
				return {};
			}

			void dumpModule() const
			{
				module->dump();
			}

			void stripInstructionsAfterTerminators();

			llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *func, llvm::Type *type, const std::string &name)
			{
				llvm::IRBuilder<> tmp(&func->getEntryBlock(), func->getEntryBlock().begin());
				return tmp.CreateAlloca(type, nullptr, name.c_str());
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
			llvm::Value *visit(ast::ASTSubscriptExpression *node);
			llvm::Value *visit(ast::ASTSubscriptRangedExpression *node);
			llvm::Value *visit(ast::ASTMemberAccessExpression *node);

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
