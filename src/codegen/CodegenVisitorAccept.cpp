// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/ControlStmt.h"
#include "ast/Expr.h"
#include "ast/FunctionStmt.h"
#include "ast/FwdDecl.h"
#include "ast/LiteralExpr.h"
#include "ast/Node.h"
#include "ast/OperatorExpr.h"
#include "ast/Stmt.h"
#include "codegen/CodegenVisitor.h"

std::unique_ptr<codegen::TypedValue>
ast::Expr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::Stmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::IfStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ForStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ForeachStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::WhileStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ImportStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ModuleStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::EmptyExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::IdentifierExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::VariableRefExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::VariableDefinitionExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::GlobalVariableDefinitionExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::FunctionParameter::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::FunctionPrototypeStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::FunctionDefinitionStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ReturnStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::IntegerLiteralExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::FloatLiteralExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::StringLiteralExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::CharLiteralExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::BoolLiteralExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::BinaryExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::UnaryExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::AssignmentExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ArbitraryOperandExpr::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}

std::unique_ptr<codegen::TypedValue>
ast::EmptyStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::BlockStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::ExprStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
std::unique_ptr<codegen::TypedValue>
ast::AliasStmt::accept(codegen::CodegenVisitor* v)
{
    return v->visit(this);
}
