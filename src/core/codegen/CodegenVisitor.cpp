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
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"

namespace core
{
	namespace codegen
	{
		CodegenVisitor::CodegenVisitor(llvm::LLVMContext &c, llvm::Module *m)
			: context{c}, builder(context), module(m),
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

		bool CodegenVisitor::codegen(ast::AST *ast)
		{
			module->setSourceFileName(ast->file);

			auto root = ast->globalNode.get();
			for(auto &child : root->nodes)
			{
				if(!child->accept(this))
				{
					util::logger->debug("visit failed, child->nodeType: {}", child->nodeType);
					return false;
				}
			}

			#if USE_LLVM_MODULE_VERIFY
			util::logger->trace("Verifying module");
			if(!llvm::verifyModule(*module))
			{
				util::logger->error("Module verification failed");
				util::logger->trace("Module dump:");
				dumpModule();
				return false;
			}
			util::logger->trace("Module verification successful");
			#endif

			return true;
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTExpression *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTStatement *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTIfStatement *stmt)
		{
			llvm::Value *cond = stmt->condition->accept(this);
			if(!cond)
			{
				return nullptr;
			}

			if(cond->getType()->isIntegerTy())
			{
				cond = builder.CreateIntCast(cond, findType("Bool"), false, "comptmp");
			}

			llvm::Function *func = builder.GetInsertBlock()->getParent();

			llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", func);
			llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else");
			llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "ifcont");

			builder.CreateCondBr(cond, thenBB, elseBB);
			builder.SetInsertPoint(thenBB);

			llvm::Value *thenV = stmt->ifBlock->accept(this);
			if(!thenV)
			{
				return nullptr;
			}

			builder.CreateBr(mergeBB);
			thenBB = builder.GetInsertBlock();

			func->getBasicBlockList().push_back(elseBB);
			builder.SetInsertPoint(elseBB);

			llvm::Value *elseV = stmt->elseBlock->accept(this);
			if(!elseV)
			{
				return nullptr;
			}

			builder.CreateBr(mergeBB);
			elseBB = builder.GetInsertBlock();

			func->getBasicBlockList().push_back(mergeBB);
			builder.SetInsertPoint(mergeBB);
			return mergeBB;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTForStatement *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTForeachStatement *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTWhileStatement *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTImportStatement *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTModuleStatement *node)
		{
			module->setModuleIdentifier(node->moduleName->value);
			return getDummyValue();
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTEmptyExpression *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTIdentifierExpression *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
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
		llvm::Value *CodegenVisitor::visit(ast::ASTCastExpression *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
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
			auto init = expr->init.get();

			llvm::Value *initVal = nullptr;
			if(init->nodeType == ast::ASTNode::EMPTY_EXPR)
			{
				initVal = llvm::UndefValue::get(type);
			}
			else
			{
				initVal = init->accept(this);
			}
			if(!initVal)
			{
				return nullptr;
			}

			Variable var = { nullptr, type, expr->name->value };
			auto alloca = createEntryBlockAlloca(func, var);
			builder.CreateStore(initVal, alloca);

			var.value = alloca;
			variables.insert({expr->name->value, var});

			return initVal;
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTFunctionParameter *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
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

			llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, stmt->name->value, module);

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

			if(func->getReturnType()->isVoidTy() &&
				stmt->body->nodes.back()->nodeType != ast::ASTNode::RETURN_STMT)
			{
				builder.CreateRetVoid();
			}

			#if USE_LLVM_FUNCTION_VERIFY
			if(!llvm::verifyFunction(*func))
			{
				codegenError("Function verification failed");
				util::logger->trace("Invalid function dump:");
				func->dump();
				func->eraseFromParent();
				return nullptr;
			}
			#endif

			return func;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTFunctionDeclarationStatement *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTReturnStatement *stmt)
		{
			if(stmt->returnValue->nodeType == ast::ASTNode::EMPTY_EXPR)
			{
				builder.CreateRetVoid();
				return getDummyValue();
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
		llvm::Constant *CodegenVisitor::visit(ast::ASTNoneLiteralExpression *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
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

			auto lhsType = lhs->getType();
			auto rhsType = rhs->getType();
			switch(expr->oper.get())
			{
			case lexer::TOKEN_OPERATORB_ADD:
				if(lhsType->isIntegerTy() && rhsType->isIntegerTy())
				{
					return builder.CreateAdd(lhs, rhs, "addtmp");
				}
				else if(lhsType->isFloatingPointTy() && rhsType->isFloatingPointTy())
				{
					return builder.CreateFAdd(lhs, rhs, "addtmp");
				}
				return codegenError("Type mismatch: Cannot perform add on Integer and Float");
			case lexer::TOKEN_OPERATORB_EQ:
				if(lhsType->isIntegerTy() && rhsType->isIntegerTy())
				{
					return builder.CreateICmpEQ(lhs, rhs, "cmpeqtmp");
				}
				else if(lhsType->isFloatingPointTy() && rhsType->isFloatingPointTy())
				{
					return builder.CreateFCmpOEQ(lhs, rhs, "cmpeqtmp");
				}
				return codegenError("Type mismatch: Cannot perform add on Integer and Float");
			default:
				return codegenError("Unimplemented binary operator");
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
		llvm::Value *CodegenVisitor::visit(ast::ASTAssignmentOperationExpression *node)
		{
			if(node->oper != lexer::TOKEN_OPERATORA_SIMPLE)
			{
				codegenWarning("Unimplemented CodegenVisitor::visit({}) for this operator", node->nodeType.get());
				return nullptr;
			}

			auto lhs = dynamic_cast<ast::ASTVariableRefExpression*>(node->lval.get());
			if(!lhs)
			{
				return codegenError("'=' requires lhs to be a variable");
			}

			auto val = lhs->accept(this);
			if(!val)
			{
				codegenWarning("Assignment operator lhs codegen failed");
				return nullptr;
			}

			auto varit = variables.find(lhs->value);
			if(varit == variables.end())
			{
				return codegenError("Undefined variable '{}'", lhs->value);
			}

			llvm::Value *var = varit->second.value;
			if(!var)
			{
				return codegenError("Variable value is null");
			}
			builder.CreateStore(val, var);
			return val;
		}

		llvm::Value *CodegenVisitor::visit(ast::ASTEmptyStatement *node)
		{
			codegenWarning("Unimplemented CodegenVisitor::visit({})", node->nodeType.get());
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTBlockStatement *stmt)
		{
			for(auto &child : stmt->nodes)
			{
				if(!child->accept(this))
				{
					codegenWarning("Block child codegen failed: {}", child->nodeType);
					return nullptr;
				}
			}
			return llvm::BasicBlock::Create(context);
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTWrappedExpressionStatement *stmt)
		{
			auto val = stmt->expr->accept(this);
			if(!val)
			{
				codegenWarning("Wrapped expression codegen failed: {}", stmt->expr->nodeType);
			}
			return val;
		}
	} // namespace codegen
} // namespace core
