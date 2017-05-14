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
class DumpVisitor final : public Visitor
{
public:
    explicit DumpVisitor(bool pVerbose = true, bool pUseError = false)
        : astlogger(util::createLogger(false, "dumpast_logger")),
          verbose(pVerbose), useError(pUseError)
    {
        if(verbose)
        {
            astlogger->set_pattern("DumpAST: %v");
        }
        else
        {
            astlogger->set_pattern("%v");
        }
        astlogger->flush_on(spdlog::level::err);
    }

    DumpVisitor(const DumpVisitor&) = delete;
    DumpVisitor& operator=(const DumpVisitor&) = delete;
    DumpVisitor(DumpVisitor&&) noexcept = default;
    DumpVisitor& operator=(DumpVisitor&&) noexcept = default;

    ~DumpVisitor() noexcept override
    {
        finish();
    }

    template <typename T>
    void dump(T* root)
    {
        start(root);
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
    bool verbose{true};
    bool useError{false};

public:
    void visit(Node* node, size_t ind = 0);
    void visit(Stmt* stmt, size_t ind = 0);
    void visit(Expr* expr, size_t ind = 0);

    void visit(IfStmt* node, size_t ind = 0);
    void visit(ForStmt* node, size_t ind = 0);
    void visit(ForeachStmt* node, size_t ind = 0);
    void visit(WhileStmt* node, size_t ind = 0);
    void visit(ImportStmt* node, size_t ind = 0);
    void visit(ModuleStmt* node, size_t ind = 0);

    void visit(EmptyExpr* node, size_t ind = 0);
    void visit(IdentifierExpr* node, size_t ind = 0);
    void visit(VariableRefExpr* node, size_t ind = 0);
    void visit(VariableDefinitionExpr* node, size_t ind = 0);
    void visit(GlobalVariableDefinitionExpr* node, size_t ind = 0);

    void visit(FunctionParameter* node, size_t ind = 0);
    void visit(FunctionPrototypeStmt* node, size_t ind = 0);
    void visit(FunctionDefinitionStmt* node, size_t ind = 0);
    void visit(ReturnStmt* node, size_t ind = 0);

    void visit(IntegerLiteralExpr* node, size_t ind = 0);
    void visit(FloatLiteralExpr* node, size_t ind = 0);
    void visit(StringLiteralExpr* node, size_t ind = 0);
    void visit(CharLiteralExpr* node, size_t ind = 0);
    void visit(BoolLiteralExpr* node, size_t ind = 0);

    void visit(BinaryExpr* node, size_t ind = 0);
    void visit(UnaryExpr* node, size_t ind = 0);
    void visit(AssignmentExpr* node, size_t ind = 0);
    void visit(ArbitraryOperandExpr* node, size_t ind = 0);

    void visit(EmptyStmt* node, size_t ind = 0);
    void visit(BlockStmt* node, size_t ind = 0);
    void visit(ExprStmt* node, size_t ind = 0);
    void visit(AliasStmt* node, size_t ind = 0);
};

template <typename... Args>
inline void DumpVisitor::log(const std::string& format, Args... args)
{
    if(useError)
    {
        astlogger->error(format.c_str(), args...);
    }
    else
    {
        astlogger->debug(format.c_str(), args...);
    }
}

template <typename... Args>
inline void DumpVisitor::log(size_t ind, const std::string& format,
                             Args... args)
{
    if(ind == 0)
    {
        log(format, args...);
    }
    else
    {
        std::string prefix(R"(\)");
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
inline void DumpVisitor::start(T* root)
{
    if(verbose)
    {
        util::loggerBasic->trace("");
        log("*** AST DUMP ***");
    }
    root->accept(this);
}

inline void DumpVisitor::finish()
{
    if(verbose)
    {
        log("*** FINISHED AST DUMP ***");
    }
    astlogger->flush();
}
} // namespace ast
