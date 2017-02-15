// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/Codegen.h"
#include "codegen/GrammarCheckerVisitor.h"
#include "util/ProgramOptions.h"
#include "util/StringUtils.h"
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <fstream>

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
    return finish();
}

bool Codegen::prepare()
{
    {
        // Check grammar
        auto ref = std::make_unique<GrammarCheckerVisitor>();
        if(!ref->run<ast::ASTBlockStatement*>(ast->globalNode.get()))
        {
            return false;
        }
    }

    // Initialize LLVM stuff
    // For some reason all of these return false
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
    // Run CodegenVisitor
    return codegen->codegen(ast.get());
}

bool Codegen::finish()
{
    util::logger->trace("Optimizing...");
    // Run Optimizer
    optimizer->init();
    optimizer->run();
    return true;
}

void Codegen::write()
{
    const auto output = util::viewProgramOptions().output;

    if(output == util::EMIT_NONE)
    {
        util::logger->info("Emitting nothing");
        return;
    }

    if(util::viewProgramOptions().outputFilename == "-")
    {
        // If output file set to stdout,
        // dump the module there
        util::loggerBasic->error("*** MODULE DUMP ***\n");
        codegen->dumpModule();
        util::loggerBasic->error("\n*** END MODULE DUMP ***");
        return;
    }

    auto filename = [&](util::OutputType type) {
        auto filenameWithoutEnding = [&](
            const std::string& f = util::viewProgramOptions().outputFilename) {
            auto filenameParts = util::stringutils::split(f, '.');
            if(filenameParts.size() > 1)
            {
                filenameParts.pop_back();
            }

            return util::stringutils::join(filenameParts, '.');
        };

        static const std::unordered_map<util::OutputType, std::string>
            filenameEndings{
                {util::EMIT_NONE, ""},       {util::EMIT_AST, ""},
                {util::EMIT_LLVM_IR, ".ll"}, {util::EMIT_LLVM_BC, ".bc"},
                {util::EMIT_OBJ, ".o"},      {util::EMIT_ASM, ".s"}};

        if(util::viewProgramOptions().outputFilename.empty())
        {
            return filenameWithoutEnding(
                       util::viewProgramOptions().inputFilenames[0])
                .append(filenameEndings.find(type)->second);
        }
        if(type == output)
        {
            return util::viewProgramOptions().outputFilename;
        }
        return filenameWithoutEnding().append(
            filenameEndings.find(type)->second);
    };

    util::logger->trace("EMIT_AST: {}", filename(util::EMIT_AST));
    util::logger->trace("EMIT_LLVM_IR: {}", filename(util::EMIT_LLVM_IR));
    util::logger->trace("EMIT_LLVM_BC: {}", filename(util::EMIT_LLVM_BC));
    util::logger->trace("EMIT_OBJ: {}", filename(util::EMIT_OBJ));
    util::logger->trace("EMIT_ASM: {}", filename(util::EMIT_ASM));
    util::logger->trace("output: {}", output);

    if(output == util::EMIT_AST)
    {
        throw std::logic_error("EMIT_AST in Codegen::write()");
    }

    std::error_code ec;
    llvm::raw_fd_ostream os(filename(util::EMIT_LLVM_IR).c_str(), ec,
                            llvm::sys::fs::F_None);

    // Throw on error
    if(ec)
    {
        throw std::runtime_error(
            fmt::format("Failed to open output file: {}", ec.message()));
    }

    // Write the module to the stream
    module->print(os, nullptr);

    if(output == util::EMIT_LLVM_IR)
    {
        util::logger->info("Wrote LLVM IR in '{}'",
                           filename(util::EMIT_LLVM_IR));
        return;
    }

    if(output == util::EMIT_LLVM_BC)
    {
        // FIXME: system(3) is BAD
        // Especially with UNSANITIZED input
        const auto retAs = std::system(fmt::format("llvm-as-3.9 -o {} {}",
                                                   filename(util::EMIT_LLVM_BC),
                                                   filename(util::EMIT_LLVM_IR))
                                           .c_str());
        std::remove(filename(util::EMIT_LLVM_IR).c_str());
        if(retAs != 0)
        {
            throw std::runtime_error("llvm-as-3.9 failed");
        }
        util::logger->info("Wrote LLVM BC in '{}'",
                           filename(util::EMIT_LLVM_BC));
        return;
    }

    // FIXME: system(3) is BAD
    // Especially with UNSANITIZED input
    auto outputType = [&]() {
        if(output == util::EMIT_OBJ)
        {
            return "obj";
        }
        return "asm";
    }();
    const auto retLlc =
        std::system(fmt::format("llc-3.9 -filetype={} -o {} {}", outputType,
                                filename(output), filename(util::EMIT_LLVM_IR))
                        .c_str());
    std::remove(filename(util::EMIT_LLVM_IR).c_str());
    if(retLlc != 0)
    {
        throw std::runtime_error("llc-3.9 failed");
    }

    util::logger->info("Wrote {} in '{}'", outputType, filename(output));
}
} // namespace codegen
