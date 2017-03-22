// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/Serializer.h"
#include "ast/ControlStmt.h"
#include "ast/DumpVisitor.h"
#include "ast/Expr.h"
#include "ast/FunctionStmt.h"
#include "ast/LiteralExpr.h"
#include "ast/Node.h"
#include "ast/OperatorExpr.h"
#include "ast/Stmt.h"
#include <cereal.h>
#include <cereal_archives.h>

CEREAL_REGISTER_TYPE(::ast::Node);

CEREAL_REGISTER_TYPE(::ast::Expr);
CEREAL_REGISTER_TYPE(::ast::ArbitraryOperandExpr);
CEREAL_REGISTER_TYPE(::ast::AssignmentExpr);
CEREAL_REGISTER_TYPE(::ast::BinaryExpr);
CEREAL_REGISTER_TYPE(::ast::BoolLiteralExpr);
CEREAL_REGISTER_TYPE(::ast::CharLiteralExpr);
CEREAL_REGISTER_TYPE(::ast::EmptyExpr);
CEREAL_REGISTER_TYPE(::ast::FloatLiteralExpr);
CEREAL_REGISTER_TYPE(::ast::IdentifierExpr);
CEREAL_REGISTER_TYPE(::ast::VariableRefExpr);
CEREAL_REGISTER_TYPE(::ast::IntegerLiteralExpr);
CEREAL_REGISTER_TYPE(::ast::StringLiteralExpr);
CEREAL_REGISTER_TYPE(::ast::UnaryExpr);
CEREAL_REGISTER_TYPE(::ast::VariableDefinitionExpr);
CEREAL_REGISTER_TYPE(::ast::GlobalVariableDefinitionExpr);

CEREAL_REGISTER_TYPE(::ast::Stmt);
CEREAL_REGISTER_TYPE(::ast::AliasStmt);
CEREAL_REGISTER_TYPE(::ast::BlockStmt);
CEREAL_REGISTER_TYPE(::ast::EmptyStmt);
CEREAL_REGISTER_TYPE(::ast::ForeachStmt);
CEREAL_REGISTER_TYPE(::ast::ForStmt);
CEREAL_REGISTER_TYPE(::ast::FunctionDefinitionStmt);
CEREAL_REGISTER_TYPE(::ast::FunctionParameter);
CEREAL_REGISTER_TYPE(::ast::FunctionPrototypeStmt);
CEREAL_REGISTER_TYPE(::ast::IfStmt);
CEREAL_REGISTER_TYPE(::ast::ImportStmt);
CEREAL_REGISTER_TYPE(::ast::ModuleStmt);
CEREAL_REGISTER_TYPE(::ast::ReturnStmt);
CEREAL_REGISTER_TYPE(::ast::WhileStmt);
CEREAL_REGISTER_TYPE(::ast::ExprStmt);

// CEREAL_REGISTER_DYNAMIC_INIT(ast);

namespace ast
{
Serializer::Serializer(std::shared_ptr<AST> a) : ast(a)
{
}

void Serializer::run(std::ostream& os, SerializeType type)
{
    if(type == BIN)
    {
        cereal::BinaryOutputArchive archive(os);
        archive(CEREAL_NVP(ast));
    }
    else if(type == XML)
    {
        cereal::XMLOutputArchive archive(os);
        archive(CEREAL_NVP(ast));
    }
    else if(type == JSON)
    {
        cereal::JSONOutputArchive archive(os);
        archive(CEREAL_NVP(ast));
    }
}

void Serializer::run(spdlog::logger& logger, spdlog::level::level_enum level,
                     SerializeType type)
{
    std::stringstream ss;
    run(ss, type);
    logger.log(level, ss.str().c_str());
}

void Serializer::runDump()
{
    DumpVisitor dumper{};
    dumper.dump(ast->globalNode.get());
}
} // namespace ast
