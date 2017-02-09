// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/FwdDecl.h"
#include "ast/Visitor.h"
#include "util/Logger.h"

namespace ast
{
/// Dump the AST to stdout
class DumpASTVisitor final : public Visitor
{
public:
    DumpASTVisitor() : astlogger(util::createLogger(false, "dumpast_logger"))
    {
        astlogger->set_pattern("DumpAST: %v");
        astlogger->flush_on(spdlog::level::warn);
    }

    ~DumpASTVisitor() override
    {
        spdlog::drop("DumpAST");
    }

    DumpASTVisitor(const DumpASTVisitor&) = delete;
    DumpASTVisitor(DumpASTVisitor&&) = default;

    DumpASTVisitor& operator=(const DumpASTVisitor&) = delete;
    DumpASTVisitor& operator=(DumpASTVisitor&&) = default;

    /**
     * Dump an AST
     * @param node Root node, must be an ASTNode or derivative
     */
    template <typename T>
    static void dump(T* node)
    {
        auto dumpAST = std::make_unique<DumpASTVisitor>();
        dumpAST->start<T>(node);
        dumpAST->finish();
    }

private:
    template <typename T>
    void start(T* root);
    void finish();

    template <typename... Args>
    void log(const std::string& format, Args... args);
    template <typename... Args>
    void log(size_t ind, const std::string& format, Args... args);

    std::shared_ptr<spdlog::logger> astlogger;

public:
    void visit(ASTNode* node, size_t ind = 0);
    void visit(ASTStatement* stmt, size_t ind = 0);
    void visit(ASTExpression* expr, size_t ind = 0);

    void visit(ASTIfStatement* node, size_t ind = 0);
    void visit(ASTForStatement* node, size_t ind = 0);
    void visit(ASTForeachStatement* node, size_t ind = 0);
    void visit(ASTWhileStatement* node, size_t ind = 0);
    void visit(ASTImportStatement* node, size_t ind = 0);
    void visit(ASTModuleStatement* node, size_t ind = 0);

    void visit(ASTEmptyExpression* node, size_t ind = 0);
    void visit(ASTIdentifierExpression* node, size_t ind = 0);
    void visit(ASTVariableRefExpression* node, size_t ind = 0);
    void visit(ASTCastExpression* node, size_t ind = 0);
    void visit(ASTVariableDefinitionExpression* node, size_t ind = 0);
    void visit(ASTGlobalVariableDefinitionExpression* node, size_t ind = 0);
    void visit(ASTSubscriptExpression* node, size_t ind = 0);
    void visit(ASTSubscriptRangedExpression* node, size_t ind = 0);
    void visit(ASTMemberAccessExpression* node, size_t ind = 0);

    void visit(ASTFunctionParameter* node, size_t ind = 0);
    void visit(ASTFunctionPrototypeStatement* node, size_t ind = 0);
    void visit(ASTFunctionDefinitionStatement* node, size_t ind = 0);
    void visit(ASTReturnStatement* node, size_t ind = 0);

    void visit(ASTIntegerLiteralExpression* node, size_t ind = 0);
    void visit(ASTFloatLiteralExpression* node, size_t ind = 0);
    void visit(ASTStringLiteralExpression* node, size_t ind = 0);
    void visit(ASTCharLiteralExpression* node, size_t ind = 0);
    void visit(ASTBoolLiteralExpression* node, size_t ind = 0);
    void visit(ASTNoneLiteralExpression* node, size_t ind = 0);

    void visit(ASTBinaryExpression* node, size_t ind = 0);
    void visit(ASTUnaryExpression* node, size_t ind = 0);
    void visit(ASTAssignmentExpression* node, size_t ind = 0);
    void visit(ASTArbitraryOperandExpression* node, size_t ind = 0);

    void visit(ASTEmptyStatement* node, size_t ind = 0);
    void visit(ASTBlockStatement* node, size_t ind = 0);
    void visit(ASTWrappedExpressionStatement* node, size_t ind = 0);
};

template <typename... Args>
inline void DumpASTVisitor::log(const std::string& format, Args... args)
{
    astlogger->debug(format.c_str(), args...);
}

template <typename... Args>
inline void DumpASTVisitor::log(size_t ind, const std::string& format,
                                Args... args)
{
    if(ind == 0)
    {
        log(format, args...);
    }
    else
    {
        std::string prefix("\\");
        size_t realind = ind * 2;
        for(size_t i = 1; i < realind; ++i)
        {
            prefix.push_back('-');
        }
        prefix.append(format);
        log(prefix, args...);
    }
}

template <typename T>
inline void DumpASTVisitor::start(T* root)
{
    util::loggerBasic->trace("");
    log("*** AST DUMP ***");
    auto castedRoot = dynamic_cast<ASTNode*>(root);
    if(!castedRoot)
    {
        throw std::invalid_argument(
            "Invalid root node given to DumpASTVisitor");
    }
    root->accept(this);
}

inline void DumpASTVisitor::finish()
{
    log("*** FINISHED AST DUMP ***");
    astlogger->flush();
}
} // namespace ast
