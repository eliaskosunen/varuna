/*
Copyright (C) 2016-2017 Elias Kosunen

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

#include "core/codegen/Optimizer.h"

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
		Optimizer::Optimizer(llvm::Module *m, const CodegenInfo &i)
			: module(m), info(i),
			fpm(std::make_unique<llvm::legacy::FunctionPassManager>(m)), mpm{std::make_unique<llvm::legacy::PassManager>()},
			optLevel(info.optLevel), sizeLevel(info.sizeLevel) {}

		Optimizer::~Optimizer()
		{
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
			for(auto &func : *module)
			{
				fpm->run(func);
			}
			fpm->add(llvm::createVerifierPass());

			mpm->add(llvm::createVerifierPass());
			mpm->run(*module);
			mpm->add(llvm::createVerifierPass());
		}

		void Optimizer::initPasses()
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

		void Optimizer::addPasses(llvm::TargetMachine *tm)
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

		void Optimizer::addLinkPasses()
		{
			llvm::PassManagerBuilder builder;
			builder.VerifyInput = true;
			builder.OptLevel = optLevel;
			builder.SizeLevel = sizeLevel;

			builder.populateLTOPassManager(*mpm);
		}
	}
}
