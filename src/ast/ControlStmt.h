// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/Expr.h"
#include "ast/FwdDecl.h"
#include "ast/Stmt.h"
#include <cereal.h>

namespace ast
{
/// if-statement
class IfStmt : public Stmt
{
    friend class cereal::access;

    IfStmt() : Stmt(IF_STMT)
    {
    }

public:
    IfStmt(std::unique_ptr<Expr> pCondition, std::unique_ptr<Stmt> pIfBlock,
           std::unique_ptr<Stmt> pElseBlock)
        : Stmt(IF_STMT), condition(std::move(pCondition)),
          ifBlock(std::move(pIfBlock)), elseBlock(std::move(pElseBlock))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(condition),
                CEREAL_NVP(ifBlock), CEREAL_NVP(elseBlock));
    }

    /// Condition
    std::unique_ptr<Expr> condition;
    /// then-block
    std::unique_ptr<Stmt> ifBlock;
    /// else-block
    /// EmptyStmt if no else-block was set
    std::unique_ptr<Stmt> elseBlock;
};

/// for-statement
class ForStmt : public Stmt
{
    friend class cereal::access;

    ForStmt() : Stmt(FOR_STMT)
    {
    }

public:
    ForStmt(std::unique_ptr<Stmt> pBlock, std::unique_ptr<Expr> pInit,
            std::unique_ptr<Expr> pEnd, std::unique_ptr<Expr> pStep)
        : Stmt(FOR_STMT), init(std::move(pInit)), end(std::move(pEnd)),
          step(std::move(pStep)), block(std::move(pBlock))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(init),
                CEREAL_NVP(end), CEREAL_NVP(step), CEREAL_NVP(block));
    }

    /// init-expression: for INIT,,
    std::unique_ptr<Expr> init;
    /// end-expression: for ,END,
    std::unique_ptr<Expr> end;
    /// step-expression: for ,,STEP
    std::unique_ptr<Expr> step;
    /// Loop body
    std::unique_ptr<Stmt> block;
};

/// foreach-statement
/// Currently unused
class ForeachStmt : public Stmt
{
    friend class cereal::access;

    ForeachStmt() : Stmt(FOREACH_STMT)
    {
    }

public:
    ForeachStmt(std::unique_ptr<Expr> pIteratee,
                std::unique_ptr<Expr> pIterator, std::unique_ptr<Stmt> pBlock)
        : Stmt(FOREACH_STMT), iteratee(std::move(pIteratee)),
          iterator(std::move(pIterator)), block(std::move(pBlock))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(iteratee),
                CEREAL_NVP(iterator), CEREAL_NVP(block));
    }

    std::unique_ptr<Expr> iteratee, iterator;
    std::unique_ptr<Stmt> block;
};

/// while-statement
class WhileStmt : public Stmt
{
    friend class cereal::access;

    WhileStmt() : Stmt(WHILE_STMT)
    {
    }

public:
    WhileStmt(std::unique_ptr<Expr> pCondition, std::unique_ptr<Stmt> pBlock)
        : Stmt(WHILE_STMT), condition(std::move(pCondition)),
          block(std::move(pBlock))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(condition),
                CEREAL_NVP(block));
    }

    /// Condition
    std::unique_ptr<Expr> condition;
    /// Loop body
    std::unique_ptr<Stmt> block;
};

/// import-statement
class ImportStmt : public Stmt
{
    friend class cereal::access;

    ImportStmt() : Stmt(IMPORT_STMT)
    {
    }

public:
    /// Type of import
    enum _ImportType
    {
        UNSPECIFIED = 0, ///< import abc
        MODULE,          ///< import module abc
        PACKAGE          ///< import package abs
    };
    using ImportType = util::SafeEnum<_ImportType>;

    ImportStmt(ImportType type, std::unique_ptr<IdentifierExpr> pImportee,
               bool pIsPath = false)
        : Stmt(IMPORT_STMT), importee(std::move(pImportee)), isPath(pIsPath),
          importType(type)
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(importee),
                CEREAL_NVP(isPath), CEREAL_NVP(importType));
    }

    /// What to import
    std::unique_ptr<IdentifierExpr> importee;
    bool isPath{false};
    /// Type of import
    ImportType importType{UNSPECIFIED};
};

/// module-satement
class ModuleStmt : public Stmt
{
    friend class cereal::access;

    ModuleStmt() : Stmt(MODULE_STMT)
    {
    }

public:
    explicit ModuleStmt(std::unique_ptr<IdentifierExpr> name)
        : Stmt(MODULE_STMT), moduleName(std::move(name))
    {
    }

    void accept(DumpVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ParentSolverVisitor* v, Node* p) override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::base_class<Stmt>(this), CEREAL_NVP(moduleName));
    }

    /// Name of the module
    std::unique_ptr<IdentifierExpr> moduleName;
};
} // namespace ast
