// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/Optimizer.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/InitializePasses.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/Pass.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

namespace codegen
{
Optimizer::Optimizer(llvm::Module* m, CodegenInfo i)
    : module(m), info(i),
      fpm(std::make_unique<llvm::legacy::FunctionPassManager>(m)),
      mpm{std::make_unique<llvm::legacy::PassManager>()}
{
}

Optimizer::~Optimizer()
{
    // Required for some reason
    // C'mon LLVM, C++ has RAII
    fpm->doFinalization();
}

void Optimizer::init()
{
    initPasses();
    addLinkPasses();
    addPasses();

    fpm->doInitialization();
}

void Optimizer::run()
{
    // Run function passes on each function of the module
    for(auto& func : *module)
    {
        fpm->run(func);
    }
    // Verify
    fpm->add(llvm::createVerifierPass());

    // Run module passes and verify
    mpm->add(llvm::createVerifierPass());
    mpm->run(*module);
    mpm->add(llvm::createVerifierPass());
}

void Optimizer::initPasses()
{
    // Initialize all LLVM passes
    // Copy-pasted from somewhere, the documentation sucks bad on this
    llvm::PassRegistry& Registry = *llvm::PassRegistry::getPassRegistry();
    llvm::initializeCore(Registry);
    llvm::initializeScalarOpts(Registry);
    llvm::initializeObjCARCOpts(Registry);
    llvm::initializeVectorization(Registry);
    llvm::initializeIPO(Registry);
    llvm::initializeAnalysis(Registry);
    llvm::initializeTransformUtils(Registry);
    llvm::initializeInstCombine(Registry);
    llvm::initializeInstrumentation(Registry);
    llvm::initializeTarget(Registry);
    llvm::initializeCodeGenPreparePass(Registry);
    llvm::initializeAtomicExpandPass(Registry);
    llvm::initializeRewriteSymbolsPass(Registry);
    llvm::initializeWinEHPreparePass(Registry);
    llvm::initializeDwarfEHPreparePass(Registry);
    llvm::initializeSafeStackPass(Registry);
    llvm::initializeSjLjEHPreparePass(Registry);
    llvm::initializePreISelIntrinsicLoweringLegacyPassPass(Registry);
    llvm::initializeGlobalMergePass(Registry);
    llvm::initializeInterleavedAccessPass(Registry);
    llvm::initializeUnreachableBlockElimLegacyPassPass(Registry);
}

void Optimizer::addPasses(llvm::TargetMachine* tm)
{
    // Based on an LLVM example program
    fpm->add(llvm::createVerifierPass());

    llvm::PassManagerBuilder builder;
    builder.OptLevel = info.optLevel;
    builder.SizeLevel = info.sizeLevel;

    if(info.optLevel >= 1)
    {
        // Inline only on >= -O1
        builder.Inliner =
            llvm::createFunctionInliningPass(info.optLevel, info.sizeLevel);
    }
    else
    {
        // Only inline `alwaysinline`s on -O0
        builder.Inliner = llvm::createAlwaysInlinerPass();
    }

    if(tm)
    {
        auto fn = [&](const llvm::PassManagerBuilder&,
                      llvm::legacy::PassManagerBase& pm) {
            tm->addEarlyAsPossiblePasses(pm);
        };
        builder.addExtension(llvm::PassManagerBuilder::EP_EarlyAsPossible, fn);
    }

    builder.populateFunctionPassManager(*fpm);
    builder.populateModulePassManager(*mpm);
}

void Optimizer::addLinkPasses()
{
    llvm::PassManagerBuilder builder;
    builder.VerifyInput = true;
    builder.OptLevel = info.optLevel;
    builder.SizeLevel = info.sizeLevel;

    builder.populateLTOPassManager(*mpm);
}
} // namespace codegen
