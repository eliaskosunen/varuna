/*
Copyright (C) 2016 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/LinkAllPasses.h"

namespace core
{
	namespace codegen
	{
		class Optimizer
		{
			llvm::Module *module;
			std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
			std::unique_ptr<llvm::legacy::PassManager> mpm;
			bool disableOptimizations;

			void initPasses()
			{
				llvm::PassRegistry &Registry = *llvm::PassRegistry::getPassRegistry();
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

			void addPasses(uint8_t optLevel, uint8_t sizeLevel, llvm::TargetMachine *tm = nullptr)
			{
				fpm->add(llvm::createVerifierPass());

				llvm::PassManagerBuilder builder;
				builder.OptLevel = optLevel;
				builder.SizeLevel = sizeLevel;

				if(optLevel > 1)
				{
					builder.Inliner = llvm::createFunctionInliningPass(optLevel, sizeLevel);
				}
				else
				{
					builder.Inliner = llvm::createAlwaysInlinerPass();
				}

				if(tm)
				{
					auto fn = [&](const llvm::PassManagerBuilder&, llvm::legacy::PassManagerBase &pm)
					{
						tm->addEarlyAsPossiblePasses(pm);
					};
					builder.addExtension(llvm::PassManagerBuilder::EP_EarlyAsPossible, fn);
				}

				builder.populateFunctionPassManager(*fpm);
				builder.populateModulePassManager(*mpm);
			}

			void addLinkPasses()
			{
				llvm::PassManagerBuilder builder;
				builder.VerifyInput = true;
				if(disableOptimizations)
				{
					builder.OptLevel = 0;
				}
				builder.populateLTOPassManager(*mpm);
			}

		public:
			Optimizer(llvm::Module *m, bool disableOpt = false)
				: module(m), fpm(std::make_unique<llvm::legacy::FunctionPassManager>(m)), mpm{std::make_unique<llvm::legacy::PassManager>()},
				disableOptimizations(disableOpt) {}

			Optimizer(const Optimizer&) = default;
			Optimizer(Optimizer&&) = default;

			Optimizer &operator =(const Optimizer&) = default;
			Optimizer &operator =(Optimizer&&) = default;

			~Optimizer()
			{
				fpm->doFinalization();
			}

			void init()
			{
				initPasses();
				addLinkPasses();
				addPasses(disableOptimizations ? 0 : 2, 0);

				fpm->doInitialization();
			}

			void run()
			{
				for(auto &func : *module)
				{
					fpm->run(func);
				}
				fpm->add(llvm::createVerifierPass());

				mpm->add(llvm::createVerifierPass());
				mpm->run(*module);
				mpm->add(llvm::createVerifierPass());
			}
		};
	}
}
