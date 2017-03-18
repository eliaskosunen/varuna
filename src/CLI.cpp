// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "CLI.h"
#include "Runner.h"
#include "util/MathUtils.h"
#include "util/StringUtils.h"
#include <llvm/Config/llvm-config.h>
#include <llvm/Support/CommandLine.h>
#include <iterator>

#ifdef VARUNA_DEBUG
#define VARUNA_BUILD_TYPE_STRING " DEBUG"
#else
#define VARUNA_BUILD_TYPE_STRING ""
#endif

static const std::string ver =
    fmt::format("{} (LLVM version: {}) Build date: {}{}",
                util::programinfo::version::toString(), LLVM_VERSION_STRING,
                util::programinfo::getBuildDate(), VARUNA_BUILD_TYPE_STRING);

#undef VARUNA_BUILD_TYPE_STRING

CLI::CLI(int pArgc, char** pArgv) : argc(pArgc), argv(pArgv)
{
}

int CLI::run()
{
    using namespace llvm;

    if(argc <= 1)
    {
        util::ProgramOptions::get().args = "";
    }
    else
    {
        util::ProgramOptions::get().args =
            util::stringutils::join(argv + 1, argv + argc, ' ');
    }

    cl::SetVersionPrinter(&showVersion);

    // Specify options
    cl::OptionCategory catGeneral("General compiler options");
    cl::OptionCategory catCodegen("Code generation options");
    cl::OptionCategory catLLVM("LLVM options");

    // Optimization level
    cl::opt<util::OptimizationLevel> optArg(
        cl::desc("Optimization level"), cl::init(util::OPT_O0),
        cl::values(
            clEnumValN(util::OPT_O0, "O0", "No optimizations (default)"),
            clEnumValN(util::OPT_O1, "O1", "Enable trivial optimizations"),
            clEnumValN(util::OPT_O2, "O2", "Enable more optimizations"),
            clEnumValN(util::OPT_O3, "O3", "Enable expensive optimizations"),
            clEnumValN(util::OPT_Os, "Os", "Enable size optimizations"),
            clEnumValN(util::OPT_Oz, "Oz",
                       "Enable maximum size optimizations")),
        cl::cat(catCodegen));
    // Logging level
    cl::opt<spdlog::level::level_enum> logArg(
        "logging", cl::desc("Logging level"), cl::init(spdlog::level::info),
        cl::values(clEnumValN(spdlog::level::trace,
                              spdlog::level::to_str(spdlog::level::trace),
                              "Internal trace messages"),
                   clEnumValN(spdlog::level::debug,
                              spdlog::level::to_str(spdlog::level::debug),
                              "Internal debugging messages"),
                   clEnumValN(spdlog::level::info,
                              spdlog::level::to_str(spdlog::level::info),
                              "Default logging level"),
                   clEnumValN(spdlog::level::warn,
                              spdlog::level::to_str(spdlog::level::warn),
                              "Log only warnings or greater"),
                   clEnumValN(spdlog::level::err,
                              spdlog::level::to_str(spdlog::level::err),
                              "Log only errors or greater"),
                   clEnumValN(spdlog::level::critical,
                              spdlog::level::to_str(spdlog::level::critical),
                              "Log only critical messages"),
                   clEnumValN(spdlog::level::off,
                              spdlog::level::to_str(spdlog::level::off),
                              "Disable all log messages")),
        cl::cat(catGeneral));
    // Jobs
    cl::opt<int> jobsArg(
        "j", cl::desc("Number of worker threads to use (Default: 1)"),
        cl::value_desc("threads"), cl::init(1), cl::cat(catGeneral));
    // License
    cl::opt<bool> licenseArg(
        "license", cl::desc("Print license and copyright information"),
        cl::init(false), cl::cat(catGeneral));
    // Output file
    cl::opt<std::string> outputFileArg("o", cl::desc("Output file"),
                                       cl::init(""), cl::cat(catGeneral));
    // Input files
    cl::opt<std::string> inputFileArg(cl::desc("Input file"),
                                      cl::value_desc("file"), cl::Positional,
                                      cl::cat(catGeneral));
    // Debugging symbols
    cl::opt<bool> debugArg("g", cl::desc("Emit debugging symbols"),
                           cl::init(false), cl::cat(catCodegen));
    // Output
    cl::opt<util::OutputType> outputArg(
        "emit", cl::desc("Output type"), cl::init(util::EMIT_OBJ),
        cl::cat(catCodegen),
        cl::values(
            clEnumValN(util::EMIT_NONE, "none", "Emit nothing"),
            clEnumValN(util::EMIT_AST, "ast", "Abstract Syntax Tree"),
            clEnumValN(util::EMIT_LLVM_IR, "llvm-ir",
                       "LLVM Intermediate Representation '.ll'"),
            clEnumValN(util::EMIT_LLVM_BC, "llvm-bc", "LLVM Bytecode '.bc'"),
            clEnumValN(util::EMIT_ASM, "asm", "Native assembly '.s'"),
            clEnumValN(util::EMIT_OBJ, "obj",
                       "Native object format '.o' (default)")));
    // x86 asm syntax
    {
        auto& map = cl::getRegisteredOptions();
        auto needle = map.find("x86-asm-syntax");
        if(needle != map.end())
        {
            needle->second->setArgStr("llvm-x86-asm-syntax");
        }
    }
    cl::opt<util::X86AsmSyntax> x86AsmArg(
        "x86-asm-syntax", cl::desc("Emitted x86 assembly syntax"),
        cl::init(util::X86_ATT),
        cl::values(
            clEnumValN(util::X86_ATT, "att", "AT&T assembly syntax"),
            clEnumValN(util::X86_INTEL, "intel", "Intel assembly syntax")),
        cl::cat(catCodegen));
    // No module file
    cl::opt<bool> noModArg("no-module", cl::desc("Don't generate module file"),
                           cl::init(false), cl::cat(catGeneral));
    // Strip debug info
    cl::opt<bool> stripDebugArg("strip-debug", cl::desc("Strip debug info"),
                                cl::init(false), cl::cat(catCodegen));
    // Strip source filename
    cl::opt<bool> stripSourceFilenameArg("strip-source-filename",
                                         cl::desc("Strip source filename"),
                                         cl::init(false), cl::cat(catCodegen));

    {
        auto arr = std::vector<const decltype(catGeneral)*>{
            &catGeneral, &catCodegen, &catLLVM};
        cl::HideUnrelatedOptions(arr);
    }
    cl::ParseCommandLineOptions(argc, argv, "Varuna Compiler");

    // Process arguments
    spdlog::set_level(logArg);

    util::ProgramOptions::get().optLevel = optArg;
    util::ProgramOptions::get().loggingLevel = logArg;

    if(licenseArg)
    {
        // License requested, show it and quit
        showLicense();
        return 0;
    }

    if(inputFileArg.empty())
    {
        util::logger->error("No input file given!");
        return -1;
    }

    // Create Runner
    int threads = jobsArg;
    if(threads < 0)
    {
        util::logger->error("Invalid number of jobs: {}", threads);
        return -1;
    }
    Runner runner(threads);

    util::ProgramOptions::get().inputFilename = std::move(inputFileArg);
    util::ProgramOptions::get().outputFilename = std::move(outputFileArg);
    util::ProgramOptions::get().output = std::move(outputArg);

    util::ProgramOptions::get().emitDebug = debugArg;

    util::ProgramOptions::get().x86asm = x86AsmArg;
    util::ProgramOptions::get().generateModuleFile =
        !static_cast<bool>(noModArg);
    util::ProgramOptions::get().stripDebug = stripDebugArg;
    util::ProgramOptions::get().stripSourceFilename = stripSourceFilenameArg;

    // Run it
    if(!runner.run())
    {
        util::logger->info("Compilation failed");
        return 1;
    }

    util::logger->info("Compilation successful");
    return 0;
}

void CLI::removeRegisteredOptions()
{
    auto& map = llvm::cl::getRegisteredOptions();
    for(auto& o : map)
    {
        if(!o.second)
        {
            continue;
        }
        const auto& first = o.first();
        if(first == "help" || first == "help-hidden" || first == "help-list" ||
           first == "help-list-hidden" || first == "version")
        {
            continue;
        }
        o.second->removeArgument();
    }
}

void CLI::showLicense() const
{
    util::loggerBasic->error("Varuna, {}\n",
                             util::programinfo::version::toString());

    util::loggerBasic->error("Copyright (C) 2016-2017 Elias Kosunen");
    util::loggerBasic->error("Published under the 3-Clause BSD License "
                             "(\"New/Modified BSD License\")");
    util::loggerBasic->error("See the file LICENSE for details");
}

void CLI::showVersion()
{
    util::loggerBasic->error("Varuna, {}", ver);
}
