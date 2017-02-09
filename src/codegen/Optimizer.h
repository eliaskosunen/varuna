// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "codegen/CodegenInfo.h"
#include <memory>

// Forward declarations to avoid inclusion of LLVM headers
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
// LLVM module optimizer
class Optimizer
{
public:
    Optimizer(llvm::Module* m, CodegenInfo i);
    ~Optimizer();

    Optimizer(const Optimizer&) = delete;
    Optimizer(Optimizer&&) = default;

    Optimizer& operator=(const Optimizer&) = delete;
    Optimizer& operator=(Optimizer&&) = default;

    /**
     * Initialize optimizer passes
     */
    void init();
    /**
     * Run the initialized passes
     */
    void run();

private:
    /**
     * Initialize LLVM passes
     */
    void initPasses();
    /**
     * Add LLVM passes based on optimization level
     * @param tm Target machine
     */
    void addPasses(llvm::TargetMachine* tm = nullptr);
    /**
     * Add LLVM LTO passes
     */
    void addLinkPasses();

    /// LLVM module
    llvm::Module* module;
    CodegenInfo info;
    /// Function pass manager
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
    /// Module pass manager
    std::unique_ptr<llvm::legacy::PassManager> mpm;
};
} // namespace codegen
