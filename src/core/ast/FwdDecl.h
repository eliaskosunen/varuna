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

#include <cstddef>

namespace core
{
	namespace ast
	{
		class AST;

		class ASTNode;

		class ASTExpression;
		class ASTAssignmentOperationExpression;
		class ASTBinaryOperationExpression;
		class ASTBoolLiteralExpression;
		class ASTCallExpression;
		class ASTCastExpression;
		class ASTCharLiteralExpression;
		class ASTEmptyExpression;
		class ASTFloatLiteralExpression;
		class ASTIdentifierExpression;
		class ASTVariableRefExpression;
		class ASTIntegerLiteralExpression;
		class ASTNoneLiteralExpression;
		class ASTStringLiteralExpression;
		class ASTUnaryOperationExpression;
		class ASTVariableDefinitionExpression;

		class ASTStatement;
		class ASTBlockStatement;
		class ASTEmptyStatement;
		class ASTForeachStatement;
		class ASTForStatement;
		class ASTFunctionDeclarationStatement;
		class ASTFunctionDefinitionStatement;
		class ASTFunctionParameter;
		class ASTFunctionPrototypeStatement;
		class ASTIfStatement;
		class ASTImportStatement;
		class ASTModuleStatement;
		class ASTReturnStatement;
		class ASTWhileStatement;
		class ASTWrappedExpressionStatement;

		class Visitor;
		class DumpASTVisitor;
		class ASTParentSolverVisitor;
	} // namespace ast

	namespace codegen
	{
		class CodegenVisitor;
		class GrammarCheckerVisitor;
	} // namespace codegen
} // namespace core

#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
