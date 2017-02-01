// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/Codegen.h"
#include "codegen/GrammarCheckerVisitor.h"
#include "util/ProgramOptions.h"
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>

namespace codegen
{
Codegen::Codegen(std::shared_ptr<ast::AST> a, CodegenInfo i)
    : ast(a), info(i), context{},
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
        util::logger->debug("LLVM native target assembly printer init failed");
        // return false;
    }
    if(!llvm::InitializeNativeTargetAsmParser())
    {
        util::logger->debug("LLVM native target assembly parser init failed");
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
    util::logger->trace("Optimizing...");
    optimizer->init();
    optimizer->run();
    return true;
}

void Codegen::write()
{
    if(util::viewProgramOptions().outputFilename == "stdout")
    {
        util::loggerBasic->error("*** MODULE DUMP ***\n");
        codegen->dumpModule();
        util::loggerBasic->error("\n*** END MODULE DUMP ***");
        return;
    }

    auto filename = util::viewProgramOptions().outputFilename;
    std::error_code ec;
    llvm::raw_fd_ostream os(filename, ec, llvm::sys::fs::F_None);

    if(ec)
    {
        throw std::runtime_error(
            fmt::format("Failed to open output file: {}", ec.message()));
    }

    module->print(os, nullptr);
}
} // namespace codegen
