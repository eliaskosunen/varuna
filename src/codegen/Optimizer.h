// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "codegen/CodegenInfo.h"
#include <memory>

namespace llvm
{
class Module;
class TargetMachine;

namespace legacy
{
    class FunctionPassManager;
    class PassManager;
} // namespace legacy
} // namespace llvm

namespace codegen
{
class Optimizer
{
public:
    Optimizer(llvm::Module* m, CodegenInfo i);
    ~Optimizer();

    Optimizer(const Optimizer&) = delete;
    Optimizer(Optimizer&&) = default;

    Optimizer& operator=(const Optimizer&) = delete;
    Optimizer& operator=(Optimizer&&) = default;

    /// Initialize optimizer passes
    void init();
    /// Run the passes
    void run();

private:
    void initPasses();
    void addPasses(llvm::TargetMachine* tm = nullptr);
    void addLinkPasses();

    llvm::Module* module;
    CodegenInfo info;
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
    std::unique_ptr<llvm::legacy::PassManager> mpm;
    uint8_t optLevel;
    uint8_t sizeLevel;
};
} // namespace codegen
