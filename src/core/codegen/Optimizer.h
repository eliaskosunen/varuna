// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/codegen/CodegenInfo.h"
#include <memory>

namespace llvm
{
class Module;
class TargetMachine;

namespace legacy
{
    class FunctionPassManager;
    class PassManager;
}
}

namespace core
{
namespace codegen
{
    class Optimizer
    {
        llvm::Module* module;
        const CodegenInfo& info;
        std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
        std::unique_ptr<llvm::legacy::PassManager> mpm;
        uint8_t optLevel;
        uint8_t sizeLevel;

        void initPasses();
        void addPasses(llvm::TargetMachine* tm = nullptr);
        void addLinkPasses();

    public:
        Optimizer(llvm::Module* m, const CodegenInfo& i);
        ~Optimizer();

        Optimizer(const Optimizer&) = delete;
        Optimizer(Optimizer&&) = default;

        Optimizer& operator=(const Optimizer&) = delete;
        Optimizer& operator=(Optimizer&&) = default;

        void init();
        void run();
    };
}
}
