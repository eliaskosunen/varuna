// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/AST.h"
#include "core/ast/FwdDecl.h"
#include "core/codegen/CodegenInfo.h"
#include "core/codegen/CodegenVisitor.h"
#include "core/codegen/Optimizer.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace core
{
namespace codegen
{
    class Codegen final
    {
    public:
        Codegen(std::unique_ptr<ast::AST> a, CodegenInfo i);

        Codegen(const Codegen&) = delete;
        Codegen(Codegen&&) noexcept = default;
        Codegen& operator=(const Codegen&) = delete;
        Codegen& operator=(Codegen&&) noexcept = default;
        ~Codegen() noexcept = default;

        bool run();

    private:
        bool prepare();
        bool visit();
        bool finish();

        std::unique_ptr<ast::AST> ast;
        CodegenInfo info;
        llvm::LLVMContext context;
        std::unique_ptr<llvm::Module> module;
        std::unique_ptr<CodegenVisitor> codegen;
        std::unique_ptr<Optimizer> optimizer;
    };
} // namespace codegen
} // namespace core
