// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/Codegen.h"
#include "codegen/GrammarCheckerVisitor.h"
#include "util/Process.h"
#include "util/ProgramInfo.h"
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
#if 0
    auto nameparts =
        util::stringutils::split(ast->file->getFilename(), '.');
    if(!nameparts.empty())
    {
        module->setModuleIdentifier(nameparts.front());
    }
#endif
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

struct OutputTypeHash
{
    template <typename T>
    size_t operator()(T t) const
    {
        return static_cast<size_t>(t);
    }
};

void Codegen::write()
{
    const auto output = util::ProgramOptions::view().output;
    const auto writeStdout = util::ProgramOptions::view().outputFilename == "-";

    if(output == util::EMIT_NONE)
    {
        util::logger->info("Emitting nothing");
        return;
    }

    auto filename = [&](util::OutputType type) {
        auto filenameWithoutEnding =
            [&](const std::string& f =
                    util::ProgramOptions::view().outputFilename) {
                auto filenameParts = util::stringutils::split(f, '.');
                if(filenameParts.size() > 1)
                {
                    filenameParts.pop_back();
                }

                return util::stringutils::join(filenameParts, '.');
            };

        static const std::unordered_map<util::OutputType, std::string,
                                        OutputTypeHash>
            filenameEndings{
                {util::EMIT_NONE, ""},       {util::EMIT_AST, ""},
                {util::EMIT_LLVM_IR, ".ll"}, {util::EMIT_LLVM_BC, ".bc"},
                {util::EMIT_OBJ, ".o"},      {util::EMIT_ASM, ".s"}};

        if(util::ProgramOptions::view().outputFilename.empty() || writeStdout)
        {
            return filenameWithoutEnding(
                       util::ProgramOptions::view().inputFilenames[0])
                .append(filenameEndings.find(type)->second);
        }
        if(type == output)
        {
            return util::ProgramOptions::view().outputFilename;
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

    util::logger->trace("cwd: {}", util::getCurrentDirectory());

    if(output == util::EMIT_AST)
    {
        throw std::logic_error("EMIT_AST in Codegen::write()");
    }

    {
        if(output == util::EMIT_LLVM_IR)
        {
            if(writeStdout)
            {
                // If output file set to stdout,
                // dump the module there
                util::loggerBasic->error("*** MODULE DUMP ***\n");
                codegen->dumpModule();
                util::loggerBasic->error("\n*** END MODULE DUMP ***");
                return;
            }
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
    }

#if 0
    {
        auto opt = fmt::format("{}{}", util::ProgramOptions::view().llvmBinDir,
                               util::ProgramOptions::view().llvmOptBin);
        auto optArgs = fmt::format(
            "-o {} -S -disable-inlining -disable-opt -strip-debug -verify {}",
            filename(util::EMIT_LLVM_IR), filename(util::EMIT_LLVM_IR));
        util::logger->debug("Running {} {}", opt, optArgs);
        auto p = util::Process(opt, optArgs);
        if(!p.spawn())
        {
            std::remove(filename(util::EMIT_LLVM_IR).c_str());
            throw std::runtime_error(fmt::format("opt ({} {}) failed: {}", opt,
                                                 optArgs, p.getErrorString()));
        }
        if(p.getReturnValue() != 0)
        {
            throw std::runtime_error(
                fmt::format("opt ({} {}) failed", opt, optArgs));
        }
    }
#endif

    if(output == util::EMIT_LLVM_IR)
    {
        util::logger->info("Wrote LLVM IR in '{}'",
                           filename(util::EMIT_LLVM_IR));
        return;
    }

    if(output == util::EMIT_LLVM_BC)
    {
        const auto as =
            fmt::format("{}{}", util::ProgramOptions::view().llvmBinDir,
                        util::ProgramOptions::view().llvmAsBin);
        const auto asArgs = fmt::format(
            "-o {} {}", writeStdout ? "-" : filename(util::EMIT_LLVM_BC),
            filename(util::EMIT_LLVM_IR), writeStdout ? " -f" : "");
        util::logger->debug("Running {} {}", as, asArgs);
        auto p = util::Process(as, asArgs);
        if(!p.spawn())
        {
            std::remove(filename(util::EMIT_LLVM_IR).c_str());
            throw std::runtime_error(fmt::format(
                "llvm-as ({} {}) failed: {}", as, asArgs, p.getErrorString()));
        }
        std::remove(filename(util::EMIT_LLVM_IR).c_str());
        if(p.getReturnValue() != 0)
        {
            throw std::runtime_error(
                fmt::format("llvm-as ({} {}) failed", as, asArgs));
        }

        if(!writeStdout)
        {
            util::logger->info("Wrote LLVM BC in '{}'",
                               filename(util::EMIT_LLVM_BC));
        }
        return;
    }

    auto outputType = [&]() {
        if(output == util::EMIT_OBJ)
        {
            return "obj";
        }
        return "asm";
    }();
    const auto llc =
        fmt::format("{}{}", util::ProgramOptions::view().llvmBinDir,
                    util::ProgramOptions::view().llvmLlcBin);
    const auto llcArgs = [&]() {
        const auto x86 = []() {
            const auto x86asm = util::ProgramOptions::view().x86asm;
            if(x86asm == util::X86_ATT)
            {
                return "att";
            }
            return "intel";
        }();
        const auto optStr = []() -> std::string {
            auto o = util::ProgramOptions::view().getOptLevel();
            if(std::get<1>(o) == 0 && std::get<0>(o) > 0)
            {
                return " " + util::ProgramOptions::view().optLevelToString();
            }
            return "";
        }();
        return fmt::format("-filetype={} -o {} {} --x86-asm-syntax={}{}",
                           outputType, writeStdout ? "-" : filename(output),
                           filename(util::EMIT_LLVM_IR), x86, optStr);
    }();

    util::logger->debug("Running {} {}", llc, llcArgs);
    auto p = util::Process(llc, llcArgs);
    if(!p.spawn())
    {
        std::remove(filename(util::EMIT_LLVM_IR).c_str());
        throw std::runtime_error(fmt::format("llc ({} {}) failed: {}", llc,
                                             llcArgs, p.getErrorString()));
    }
    std::remove(filename(util::EMIT_LLVM_IR).c_str());
    if(p.getReturnValue() != 0)
    {
        throw std::runtime_error(
            fmt::format("llc ({} {}) failed", llc, llcArgs));
    }
    if(!writeStdout)
    {
        util::logger->info("Wrote {} in '{}'", outputType, filename(output));
    }
}
} // namespace codegen
