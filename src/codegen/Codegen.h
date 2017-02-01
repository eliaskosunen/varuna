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

    /// Run it
    bool run();

    void write();

private:
    /// Initialize code generation
    bool prepare();
    /// Run CodegenVisitor
    bool visit();
    /// Optimize
    bool finish();

    std::shared_ptr<ast::AST> ast;
    CodegenInfo info;
    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<CodegenVisitor> codegen;
    std::unique_ptr<Optimizer> optimizer;
};
} // namespace codegen
