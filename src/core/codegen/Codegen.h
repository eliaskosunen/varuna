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

#include "core/ast/AST.h"
#include "core/ast/FwdDecl.h"
#include "core/codegen/CodegenVisitor.h"
#include "core/codegen/Optimizer.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

namespace core
{
	namespace codegen
	{
		class Codegen final
		{
			std::unique_ptr<ast::AST> ast;
			llvm::LLVMContext context;
			std::unique_ptr<llvm::Module> module;
			std::unique_ptr<CodegenVisitor> codegen;
			std::unique_ptr<Optimizer> optimizer;

			bool prepare();
			bool visit();
			bool finish();

		public:
			Codegen(std::unique_ptr<ast::AST> a);

			Codegen(const Codegen&) = delete;
			Codegen(Codegen&&) = default;
			Codegen &operator =(const Codegen&) = delete;
			Codegen &operator =(Codegen&&) = default;
			~Codegen() = default;

			bool run();
		};
	} // namespace codegen
} // namespace core
