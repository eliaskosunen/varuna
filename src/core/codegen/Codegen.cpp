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

#include "core/codegen/Codegen.h"
#include "core/codegen/GrammarCheckerVisitor.h"
#include <llvm/Support/TargetSelect.h>

namespace core
{
namespace codegen
{
    Codegen::Codegen(std::unique_ptr<ast::AST> a, CodegenInfo i)
        : ast(std::move(a)), info(std::move(i)), context{},
          module(std::make_unique<llvm::Module>("Varuna", context)),
          codegen(std::make_unique<CodegenVisitor>(context, module.get(), i)),
          optimizer(std::make_unique<Optimizer>(module.get(), i))
    {
    }

    bool Codegen::run()
    {
        if(!prepare())
        {
            return false;
        }
        if(!visit())
        {
            return false;
        }
        if(!finish())
        {
            return false;
        }
        return true;
    }

    bool Codegen::prepare()
    {
        {
            auto ref = std::make_unique<GrammarCheckerVisitor>();
            if(!ref->run<ast::ASTBlockStatement*>(ast->globalNode.get()))
            {
                return false;
            }
        }

        if(!llvm::InitializeNativeTarget())
        {
            util::logger->debug("LLVM native target init failed");
            // return false;
        }
        if(!llvm::InitializeNativeTargetAsmPrinter())
        {
            util::logger->debug(
                "LLVM native target assembly printer init failed");
            // return false;
        }
        if(!llvm::InitializeNativeTargetAsmParser())
        {
            util::logger->debug(
                "LLVM native target assembly parser init failed");
            // return false;
        }

        return true;
    }

    bool Codegen::visit()
    {
        return codegen->codegen(ast.get());
    }

    bool Codegen::finish()
    {
        util::loggerBasic->info("\nModule dump:");
        codegen->dumpModule();

        util::logger->trace("Optimizing...");
        optimizer->init();
        optimizer->run();

        util::loggerBasic->info("\nOptimized module dump:");
        codegen->dumpModule();
        return true;
    }
} // namespace codegen
} // namespace core
