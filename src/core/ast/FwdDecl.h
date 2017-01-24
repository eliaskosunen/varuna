// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <cstddef>

namespace core
{
namespace ast
{
    class AST;

    class ASTNode;

    class ASTExpression;
    class ASTArbitraryOperationExpression;
    class ASTAssignmentOperationExpression;
    class ASTBinaryOperationExpression;
    class ASTBoolLiteralExpression;
    class ASTCastExpression;
    class ASTCharLiteralExpression;
    class ASTEmptyExpression;
    class ASTFloatLiteralExpression;
    class ASTIdentifierExpression;
    class ASTVariableRefExpression;
    class ASTIntegerLiteralExpression;
    class ASTMemberAccessExpression;
    class ASTNoneLiteralExpression;
    class ASTStringLiteralExpression;
    class ASTSubscriptExpression;
    class ASTSubscriptRangedExpression;
    class ASTUnaryOperationExpression;
    class ASTVariableDefinitionExpression;
    class ASTGlobalVariableDefinitionExpression;

    class ASTStatement;
    class ASTBlockStatement;
    class ASTEmptyStatement;
    class ASTForeachStatement;
    class ASTForStatement;
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
    struct TypedValue;
} // namespace codegen
} // namespace core

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
