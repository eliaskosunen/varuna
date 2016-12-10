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
			: context{}, builder(context), module{}, namedValues{}
		{

		}

		llvm::Value *CodegenVisitor::visit(ast::ASTExpression*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTIfStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTForStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTForeachStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTWhileStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTImportStatement*)
		{
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTModuleStatement*)
		{

		}

		llvm::Value *CodegenVisitor::visit(ast::ASTEmptyExpression*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTIdentifierExpression*)
		{

		}
		llvm::LoadInst *CodegenVisitor::visit(ast::ASTVariableRefExpression *expr)
		{
			llvm::Value *v = namedValues[expr->value];
			if(!v)
			{
				return codegenError("Undefined variable: '{}'", expr->value);
			}
			return builder.CreateLoad(v, expr->value.c_str());
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTCallExpression*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTCastExpression*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTVariableDefinitionExpression*)
		{

		}

		llvm::Value *CodegenVisitor::visit(ast::ASTFunctionParameter*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTFunctionPrototypeStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTFunctionDefinitionStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTFunctionDeclarationStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTReturnStatement*)
		{

		}

		llvm::ConstantInt *CodegenVisitor::visit(ast::ASTIntegerLiteralExpression *expr)
		{
			auto type = [&]() -> llvm::IntegerType*
			{
				if(expr->mod.isSet(lexer::INTEGER_INT64))
					return llvm::Type::getInt64Ty(context);
				if(expr->mod.isSet(lexer::INTEGER_INT16))
					return llvm::Type::getInt16Ty(context);
				if(expr->mod.isSet(lexer::INTEGER_INT8))
					return llvm::Type::getInt8Ty(context);
				return llvm::Type::getInt32Ty(context);
			};
			return llvm::ConstantInt::getSigned(type(), expr->value);
		}
		llvm::Constant *CodegenVisitor::visit(ast::ASTFloatLiteralExpression *expr)
		{
			auto type = [&]() -> llvm::Type*
			{
				if(expr->mod.isSet(lexer::FLOAT_DOUBLE))
					return llvm::Type::getDoubleTy(context);
				return llvm::Type::getFloatTy(context);
			};
			return llvm::ConstantFP::get(type(), expr->value);
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
		llvm::Value *CodegenVisitor::visit(ast::ASTNoneLiteralExpression*)
		{

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
					uint32_t width = lhsW;
					llvm::Value *lhsNew = lhs;
					llvm::Value *rhsNew = rhs;
					if(lhsW < rhsW)
					{
						width = rhsW;
						codegenWarning("Implicit cast: Casting from Int{} to Int{}", lhsW, rhsW);
						lhsNew = builder.CreateIntCast(lhs, llvm::Type::getIntNTy(context, rhsW), true, "casttmp");
					}
					else if(lhsW > rhsW)
					{
						//width = lhsW // Already done in init
						codegenWarning("Implicit cast: Casting from Int{} to Int{}", rhsW, lhsW);
						rhsNew = builder.CreateIntCast(rhs, llvm::Type::getIntNTy(context, lhsW), true, "casttmp");
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

		}

		llvm::Value *CodegenVisitor::visit(ast::ASTEmptyStatement*)
		{

		}
		llvm::Value *CodegenVisitor::visit(ast::ASTBlockStatement *stmt)
		{
			auto &children = stmt->nodes;
			for(auto &&child : children)
			{
				child->accept(this);
			}
			return nullptr;
		}
		llvm::Value *CodegenVisitor::visit(ast::ASTWrappedExpressionStatement*)
		{

		}
	}
}
