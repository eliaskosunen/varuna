// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTExpression.h"
#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"

namespace core
{
namespace ast
{
    class ASTIfStatement : public ASTStatement
    {
    public:
        std::unique_ptr<ASTExpression> condition;
        std::unique_ptr<ASTStatement> ifBlock, elseBlock;

        ASTIfStatement(std::unique_ptr<ASTExpression> cond,
                       std::unique_ptr<ASTStatement> ifb,
                       std::unique_ptr<ASTStatement> elseb = nullptr)
            : ASTStatement(IF_STMT), condition(std::move(cond)),
              ifBlock(std::move(ifb)), elseBlock(std::move(elseb))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTForStatement : public ASTStatement
    {
    public:
        std::unique_ptr<ASTExpression> init, rangeDecl, rangeInit;
        std::unique_ptr<ASTStatement> block;

        ASTForStatement(std::unique_ptr<ASTStatement> stmt,
                        std::unique_ptr<ASTExpression> _init = nullptr,
                        std::unique_ptr<ASTExpression> _rangeDecl = nullptr,
                        std::unique_ptr<ASTExpression> _rangeInit = nullptr)
            : ASTStatement(FOR_STMT), init(std::move(_init)),
              rangeDecl(std::move(_rangeDecl)),
              rangeInit(std::move(_rangeInit)), block(std::move(stmt))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTForeachStatement : public ASTStatement
    {
    public:
        std::unique_ptr<ASTExpression> iteratee, iterator;
        std::unique_ptr<ASTBlockStatement> block;

        ASTForeachStatement(std::unique_ptr<ASTExpression> _iteratee,
                            std::unique_ptr<ASTExpression> _iterator,
                            std::unique_ptr<ASTBlockStatement> _block)
            : ASTStatement(FOREACH_STMT), iteratee(std::move(_iteratee)),
              iterator(std::move(_iterator)), block(std::move(_block))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTWhileStatement : public ASTStatement
    {
    public:
        std::unique_ptr<ASTExpression> condition;
        std::unique_ptr<ASTBlockStatement> block;

        ASTWhileStatement(std::unique_ptr<ASTExpression> cond,
                          std::unique_ptr<ASTBlockStatement> _block)
            : ASTStatement(WHILE_STMT), condition(std::move(cond)),
              block(std::move(_block))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTImportStatement : public ASTStatement
    {
    public:
        std::unique_ptr<ASTIdentifierExpression> importee;
        bool isPath;

        enum ImportType
        {
            UNSPECIFIED = 0,
            MODULE,
            PACKAGE
        } importType;

        ASTImportStatement(ImportType type,
                           std::unique_ptr<ASTIdentifierExpression> toImport,
                           bool _isPath = false)
            : ASTStatement(IMPORT_STMT), importee(std::move(toImport)),
              isPath(_isPath), importType(type)
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };

    class ASTModuleStatement : public ASTStatement
    {
    public:
        std::unique_ptr<ASTIdentifierExpression> moduleName;

        ASTModuleStatement(std::unique_ptr<ASTIdentifierExpression> name)
            : ASTStatement(MODULE_STMT), moduleName(std::move(name))
        {
        }

        void accept(DumpASTVisitor* v, size_t ind = 0) override;
        std::unique_ptr<codegen::TypedValue>
        accept(codegen::CodegenVisitor* v) override;
        void accept(ASTParentSolverVisitor* v, ASTNode* p) override;
        bool accept(codegen::GrammarCheckerVisitor* v) override;
    };
} // namespace ast
} // namespace core
