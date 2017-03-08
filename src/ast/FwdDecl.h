// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

namespace ast
{
class AST;

class Node;

class Expr;
class ArbitraryOperandExpr;
class AssignmentExpr;
class BinaryExpr;
class BoolLiteralExpr;
class CharLiteralExpr;
class EmptyExpr;
class FloatLiteralExpr;
class IdentifierExpr;
class VariableRefExpr;
class IntegerLiteralExpr;
class StringLiteralExpr;
class UnaryExpr;
class VariableDefinitionExpr;
class GlobalVariableDefinitionExpr;

class Stmt;
class AliasStmt;
class BlockStmt;
class EmptyStmt;
class ForeachStmt;
class ForStmt;
class FunctionDefinitionStmt;
class FunctionParameter;
class FunctionPrototypeStmt;
class IfStmt;
class ImportStmt;
class ModuleStmt;
class ReturnStmt;
class WhileStmt;
class ExprStmt;

class Visitor;
class DumpVisitor;
class ParentSolverVisitor;
class Serializer;
} // namespace ast

namespace codegen
{
class CodegenVisitor;
class GrammarCheckerVisitor;
struct TypedValue;
} // namespace codegen
