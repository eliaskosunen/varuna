// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/FwdDecl.h"
#include "ast/Stmt.h"
#include <cereal.h>

namespace ast
{
/// Function parameter
class FunctionParameter : public Stmt
{
    friend class cereal::access;

    FunctionParameter() : Stmt(FUNCTION_PARAMETER)
    {
    }

public:
    FunctionParameter(std::unique_ptr<VariableDefinitionExpr> pVar,
                      uint32_t pNum)
        : Stmt(FUNCTION_PARAMETER), var(std::move(pVar)), num(pNum)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(var),
                CEREAL_NVP(num));
    }

    /// Parameter
    std::unique_ptr<VariableDefinitionExpr> var;
    /// 1 = first parameter, 2 = second parameter...
    uint32_t num{0};
};

/// Function prototype
class FunctionPrototypeStmt : public Stmt
{
    friend class cereal::access;

    FunctionPrototypeStmt() : Stmt(FUNCTION_PROTO_STMT)
    {
    }

public:
    FunctionPrototypeStmt(
        std::unique_ptr<IdentifierExpr> pName,
        std::unique_ptr<IdentifierExpr> retType,
        std::vector<std::unique_ptr<FunctionParameter>> pParams)
        : Stmt(FUNCTION_PROTO_STMT), name(std::move(pName)),
          returnType(std::move(retType)), params(std::move(pParams))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(name),
                CEREAL_NVP(returnType), CEREAL_NVP(params), CEREAL_NVP(isMain));
    }

    /// Function name
    std::unique_ptr<IdentifierExpr> name;
    /// Function return type
    std::unique_ptr<IdentifierExpr> returnType;
    /// Function parameters
    std::vector<std::unique_ptr<FunctionParameter>> params;
    /// Is function a main function
    bool isMain{false};
};

/// Function definition
class FunctionDefinitionStmt : public Stmt
{
    friend class cereal::access;

    FunctionDefinitionStmt() : Stmt(FUNCTION_DEF_STMT)
    {
    }

public:
    FunctionDefinitionStmt(std::unique_ptr<FunctionPrototypeStmt> pProto,
                           std::unique_ptr<BlockStmt> pBody)
        : Stmt(FUNCTION_DEF_STMT), proto(std::move(pProto)),
          body(std::move(pBody))
    {
    }

    FunctionDefinitionStmt(std::unique_ptr<FunctionPrototypeStmt> pProto,
                           std::unique_ptr<EmptyStmt> pBody)
        : Stmt(FUNCTION_DEF_STMT), proto(std::move(pProto)),
          body(std::make_unique<BlockStmt>()), isDecl(true)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(proto),
                CEREAL_NVP(body), CEREAL_NVP(isDecl));
    }

    /// Function prototype
    std::unique_ptr<FunctionPrototypeStmt> proto;
    /// Function body
    /// Empty block statement if declaration
    std::unique_ptr<BlockStmt> body;
    /// Is a function declaration
    bool isDecl{false};
};

/// return-statement
class ReturnStmt : public Stmt
{
    friend class cereal::access;

    ReturnStmt() : Stmt(RETURN_STMT)
    {
    }

public:
    explicit ReturnStmt(std::unique_ptr<Expr> retval)
        : Stmt(RETURN_STMT), returnValue(std::move(retval))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(returnValue));
    }

    /// Return value
    std::unique_ptr<Expr> returnValue;
};
} // namespace ast
