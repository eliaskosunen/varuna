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

#include "core/codegen/CodegenVisitor.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"

namespace core
{
	namespace codegen
	{
		CodegenVisitor::CodegenVisitor()
			: context{}, builder(context), module(std::make_unique<llvm::Module>("Module", context)),
			variables{}, globalVariables{}, functionProtos{}, types{}
		{
			types.insert({ "Void", {llvm::Type::getVoidTy(context)} });
			types.insert({ "Integer", {llvm::Type::getInt32Ty(context)} });
			types.insert({ "Int8", {llvm::Type::getInt8Ty(context)} });
			types.insert({ "Int16", {llvm::Type::getInt16Ty(context)} });
			types.insert({ "Int32", {llvm::Type::getInt32Ty(context)} });
			types.insert({ "Int64", {llvm::Type::getInt64Ty(context)} });
			types.insert({ "Float", {llvm::Type::getFloatTy(context)} });
			types.insert({ "Double", {llvm::Type::getDoubleTy(context)} });
			types.insert({ "Bool", {llvm::Type::getInt1Ty(context)} });
			types.insert({ "Char", {llvm::Type::getInt8Ty(context)} });

			types.insert({ "String",
				{ llvm::StructType::create(context, {
					llvm::Type::getInt8PtrTy(context),
					llvm::Type::getInt32Ty(context)
				}, "String") }
			});
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTExpression*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTIfStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTForStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTForeachStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTWhileStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTImportStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTModuleStatement*)
		{
			return nullptr;
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTEmptyExpression*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTIdentifierExpression*)
		{
			return nullptr;
		}
		llvm::LoadInst *CodegenVisitor::visit(ast::ASTVariableRefExpression *expr)
		{
			auto var = variables.find(expr->value);
			if(var == variables.end())
			{
				var = globalVariables.find(expr->value);
				if(var == globalVariables.end())
				{
					return codegenError("Undefined variable: '{}'", expr->value);
				}
			}
			return builder.CreateLoad(var->second.type, var->second.value, expr->value.c_str());
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTCallExpression *expr)
		{
			llvm::Function *callee = findFunction(expr->callee->value);
			if(!callee)
			{
				return nullptr;
			}

			if(callee->arg_size() != expr->params.size())
			{
				return codegenError("{} expects {} arguments, {} provided", expr->callee->value, callee->arg_size(), expr->params.size());
			}

			std::vector<llvm::Value*> args;
			for(auto &arg : expr->params)
			{
				args.push_back(arg->accept(this));
				if(!args.back())
				{
					return nullptr;
				}
			}

			return builder.CreateCall(callee, args, "calltmp");
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTCastExpression*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTVariableDefinitionExpression *expr)
		{
			llvm::Function *func = builder.GetInsertBlock()->getParent();

			llvm::Type *type = findType(expr->type->value);
			if(!type)
			{
				return nullptr;
			}
			auto *init = expr->init.get();

			llvm::Value *initVal = nullptr;
			if(init)
			{
				initVal = init->accept(this);
				if(!initVal)
				{
					return nullptr;
				}
			}

			Variable var = { nullptr, type, expr->name->value };
			auto *alloca = createEntryBlockAlloca(func, var);
			builder.CreateStore(initVal, alloca);

			var.value = alloca;
			variables.insert({expr->name->value, var});

			return initVal;
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTFunctionParameter*)
		{
			return nullptr;
		}
		llvm::Function *CodegenVisitor::visit(ast::ASTFunctionPrototypeStatement *stmt)
		{
			std::vector<llvm::Type*> args;
			args.reserve(stmt->params.size());
			for(const auto &arg : stmt->params)
			{
				auto type = findType(arg->var->type->value);
				if(!type)
				{
					return nullptr;
				}
				args.push_back(type);
			}

			auto retType = findType(stmt->returnType->value);
			if(!retType)
			{
				return nullptr;
			}
			llvm::FunctionType *ft = llvm::FunctionType::get(retType, args, false);

			llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, stmt->name->value, module.get());

			size_t i = 0;
			for(auto &arg : f->args())
			{
				arg.setName(stmt->params[i]->var->name->value);
			}

			return f;
		}
		llvm::Function *CodegenVisitor::visit(ast::ASTFunctionDefinitionStatement *stmt)
		{
			auto name = stmt->proto->name->value;
			functionProtos[stmt->proto->name->value] = std::move(stmt->proto);
			auto func = findFunction(name);
			if(!func)
			{
				codegenWarning("Invalid function");
				return nullptr;
			}

			auto entry = llvm::BasicBlock::Create(context, "entry", func);
			builder.SetInsertPoint(entry);

			variables.clear();
			for(auto &arg : func->args())
			{
				auto alloca = createEntryBlockAlloca(func, Variable{ nullptr, arg.getType(), arg.getName() });
				builder.CreateStore(&arg, alloca);
				variables.insert({ arg.getName(), Variable{ alloca, arg.getType(), arg.getName() } });
			}

			if(!stmt->body->accept(this))
			{
				func->eraseFromParent();
				codegenWarning("Invalid function body");
				return nullptr;
			}

			if(func->getReturnType()->isVoidTy())
			{
				builder.CreateRetVoid();
			}
			if(!llvm::verifyFunction(*func))
			{
				func->eraseFromParent();
				return codegenError("Function verification failed");
			}
			return func;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTFunctionDeclarationStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTReturnStatement *stmt)
		{
			if(stmt->nodeType == ast::ASTNode::EMPTY_EXPR)
			{
				builder.CreateRetVoid();
				return llvm::ConstantInt::getSigned(llvm::Type::getInt1Ty(context), 1);
			}
			auto ret = stmt->returnValue->accept(this);
			if(!ret)
			{
				return nullptr;
			}
			builder.CreateRet(ret);
			return ret;
		}

		llvm::Constant *CodegenVisitor::visit(ast::ASTIntegerLiteralExpression *expr)
		{
			return llvm::ConstantInt::getSigned(findType(expr->type->value), expr->value);
		}
		llvm::Constant *CodegenVisitor::visit(ast::ASTFloatLiteralExpression *expr)
		{
			return llvm::ConstantFP::get(findType(expr->type->value), expr->value);
		}
		llvm::Constant *CodegenVisitor::visit(ast::ASTStringLiteralExpression *expr)
		{
			return llvm::ConstantDataArray::getString(context, expr->value);
		}
		llvm::ConstantInt *CodegenVisitor::visit(ast::ASTCharLiteralExpression *expr)
		{
			return llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), expr->value);
		}
		llvm::ConstantInt *CodegenVisitor::visit(ast::ASTBoolLiteralExpression *expr)
		{
			return llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), expr->value);
		}
		llvm::Constant *CodegenVisitor::visit(ast::ASTNoneLiteralExpression*)
		{
			return nullptr;
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTBinaryOperationExpression *expr)
		{
			llvm::Value *lhs = expr->left->accept(this);
			llvm::Value *rhs = expr->right->accept(this);
			if(!lhs || !rhs)
			{
				return nullptr;
			}

			auto *lhsType = lhs->getType();
			auto *rhsType = rhs->getType();
			switch(expr->oper.get())
			{
			case lexer::TOKEN_OPERATORB_ADD:
			{
				if(lhsType->isIntegerTy() && rhsType->isIntegerTy())
				{
					uint32_t lhsW = lhsType->getIntegerBitWidth();
					uint32_t rhsW = rhsType->getIntegerBitWidth();
					llvm::Value *lhsNew = lhs;
					llvm::Value *rhsNew = rhs;
					if(lhsW < rhsW)
					{
						codegenWarning("Implicit cast: Casting from Int{} to Int{}", rhsW, lhsW);
						rhsNew = builder.CreateIntCast(rhs, llvm::Type::getIntNTy(context, lhsW), true, "casttmp");
					}
					else if(lhsW > rhsW)
					{
						codegenWarning("Implicit cast: Casting from Int{} to Int{}", lhsW, rhsW);
						lhsNew = builder.CreateIntCast(lhs, llvm::Type::getIntNTy(context, rhsW), true, "casttmp");
					}
					return builder.CreateAdd(lhsNew, rhsNew, "addtmp");
				}
				return nullptr;
			}
			default:
				return nullptr;
			}
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTUnaryOperationExpression *expr)
		{
			llvm::Value *operand = expr->operand->accept(this);
			if(!operand)
			{
				return nullptr;
			}

			switch(expr->oper.get())
			{
			case lexer::TOKEN_OPERATORU_PLUS:
				return nullptr; // TODO
			case lexer::TOKEN_OPERATORU_MINUS:
				return nullptr; // TODO
			case lexer::TOKEN_OPERATORU_NOT:
				return nullptr; // TODO
			default:
				return nullptr; // TODO
			}
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTAssignmentOperationExpression*)
		{
			return nullptr;
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTEmptyStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTBlockStatement *stmt)
		{
			for(auto &child : stmt->nodes)
			{
				if(!child->accept(this))
				{
					return nullptr;
				}
			}
			return llvm::BasicBlock::Create(context);
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTWrappedExpressionStatement *stmt)
		{
			return stmt->expr->accept(this);
		}
	}
}
