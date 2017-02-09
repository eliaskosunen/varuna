// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTExpression.h"
#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"

namespace ast
{
/// if-statement
class ASTIfStatement : public ASTStatement
{
public:
    ASTIfStatement(std::unique_ptr<ASTExpression> pCondition,
                   std::unique_ptr<ASTStatement> pIfBlock,
                   std::unique_ptr<ASTStatement> pElseBlock)
        : ASTStatement(IF_STMT), condition(std::move(pCondition)),
          ifBlock(std::move(pIfBlock)), elseBlock(std::move(pElseBlock))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Condition
    std::unique_ptr<ASTExpression> condition;
    /// then-block
    std::unique_ptr<ASTStatement> ifBlock;
    /// else-block
    /// ASTEmptyStatement if no else-block was set
    std::unique_ptr<ASTStatement> elseBlock;
};

/// for-statement
class ASTForStatement : public ASTStatement
{
public:
    ASTForStatement(std::unique_ptr<ASTStatement> pBlock,
                    std::unique_ptr<ASTExpression> pInit,
                    std::unique_ptr<ASTExpression> pEnd,
                    std::unique_ptr<ASTExpression> pStep)
        : ASTStatement(FOR_STMT), init(std::move(pInit)), end(std::move(pEnd)),
          step(std::move(pStep)), block(std::move(pBlock))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// init-expression: for INIT,,
    std::unique_ptr<ASTExpression> init;
    /// end-expression: for ,END,
    std::unique_ptr<ASTExpression> end;
    /// step-expression: for ,,STEP
    std::unique_ptr<ASTExpression> step;
    /// Loop body
    std::unique_ptr<ASTStatement> block;
};

/// foreach-statement
/// Currently unused
class ASTForeachStatement : public ASTStatement
{
public:
    ASTForeachStatement(std::unique_ptr<ASTExpression> pIteratee,
                        std::unique_ptr<ASTExpression> pIterator,
                        std::unique_ptr<ASTStatement> pBlock)
        : ASTStatement(FOREACH_STMT), iteratee(std::move(pIteratee)),
          iterator(std::move(pIterator)), block(std::move(pBlock))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    std::unique_ptr<ASTExpression> iteratee, iterator;
    std::unique_ptr<ASTStatement> block;
};

/// while-statement
class ASTWhileStatement : public ASTStatement
{
public:
    ASTWhileStatement(std::unique_ptr<ASTExpression> pCondition,
                      std::unique_ptr<ASTStatement> pBlock)
        : ASTStatement(WHILE_STMT), condition(std::move(pCondition)),
          block(std::move(pBlock))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Condition
    std::unique_ptr<ASTExpression> condition;
    /// Loop body
    std::unique_ptr<ASTStatement> block;
};

/// import-statement
class ASTImportStatement : public ASTStatement
{
public:
    /// Type of import
    enum _ImportType
    {
        UNSPECIFIED = 0, ///< import abc
        MODULE,          ///< import module abc
        PACKAGE          ///< import package abs
    };
    using ImportType = util::SafeEnum<_ImportType>;

    ASTImportStatement(ImportType type,
                       std::unique_ptr<ASTIdentifierExpression> pImportee,
                       bool pIsPath = false)
        : ASTStatement(IMPORT_STMT), importee(std::move(pImportee)),
          isPath(pIsPath), importType(type)
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// What to import
    std::unique_ptr<ASTIdentifierExpression> importee;
    bool isPath;
    /// Type of import
    ImportType importType;
};

/// module-satement
class ASTModuleStatement : public ASTStatement
{
public:
    explicit ASTModuleStatement(std::unique_ptr<ASTIdentifierExpression> name)
        : ASTStatement(MODULE_STMT), moduleName(std::move(name))
    {
    }

    void accept(DumpASTVisitor* v, size_t ind = 0) override;
    std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) override;
    void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
    bool accept(codegen::GrammarCheckerVisitor* v) override;

    /// Name of the module
    std::unique_ptr<ASTIdentifierExpression> moduleName;
};
} // namespace ast
