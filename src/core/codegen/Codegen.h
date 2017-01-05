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
        std::unique_ptr<ast::AST> ast;
        CodegenInfo info;
        llvm::LLVMContext context;
        std::unique_ptr<llvm::Module> module;
        std::unique_ptr<CodegenVisitor> codegen;
        std::unique_ptr<Optimizer> optimizer;

        bool prepare();
        bool visit();
        bool finish();

    public:
        Codegen(std::unique_ptr<ast::AST> a, CodegenInfo i);

        Codegen(const Codegen&) = delete;
        Codegen(Codegen&&) = default;
        Codegen& operator=(const Codegen&) = delete;
        Codegen& operator=(Codegen&&) = default;
        ~Codegen() = default;

        bool run();
    };
} // namespace codegen
} // namespace core
