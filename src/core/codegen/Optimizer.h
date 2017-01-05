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

#pragma once

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

#include "core/codegen/CodegenInfo.h"

#include <memory>

namespace core
{
	namespace codegen
	{
		class Optimizer
		{
			llvm::Module *module;
			const CodegenInfo &info;
			std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
			std::unique_ptr<llvm::legacy::PassManager> mpm;
			uint8_t optLevel;
			uint8_t sizeLevel;

			void initPasses();
			void addPasses(llvm::TargetMachine *tm = nullptr);
			void addLinkPasses();

		public:
			Optimizer(llvm::Module *m, const CodegenInfo &i);
			~Optimizer();

			Optimizer(const Optimizer&) = delete;
			Optimizer(Optimizer&&) = default;

			Optimizer &operator =(const Optimizer&) = delete;
			Optimizer &operator =(Optimizer&&) = default;

			void init();
			void run();
		};
	}
}
