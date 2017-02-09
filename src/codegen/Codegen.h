// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "ast/FwdDecl.h"
#include "codegen/CodegenInfo.h"
#include "codegen/CodegenVisitor.h"
#include "codegen/Optimizer.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace codegen
{
/// Code generator
class Codegen final
{
public:
    Codegen(std::shared_ptr<ast::AST> a, CodegenInfo i);

    Codegen(const Codegen&) = delete;
    Codegen(Codegen&&) = default;
    Codegen& operator=(const Codegen&) = delete;
    Codegen& operator=(Codegen&&) = default;
    ~Codegen() noexcept = default;

    /**
     * Run the code generator
     * @return Success
     */
    bool run();

    /**
     * Write the generated code to stdout or a file based on command line
     * arguments
     */
    void write();

private:
    /**
     * Initialize the code generator
     * @return Success
     */
    bool prepare();
    /**
     * Run CodegenVisitor
     * @return Success
     */
    bool visit();
    /**
     * Optimize the generated module
     * @return Success
     */
    bool finish();

    /// AST
    std::shared_ptr<ast::AST> ast;
    CodegenInfo info;
    /// LLVM context
    llvm::LLVMContext context;
    /// LLVM module
    std::unique_ptr<llvm::Module> module;
    /// CodegenVisitor
    std::unique_ptr<CodegenVisitor> codegen;
    /// Module optimizer
    std::unique_ptr<Optimizer> optimizer;
};
} // namespace codegen
