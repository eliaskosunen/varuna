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
        util::getProgramOptions().args = "";
    }
    else
    {
        util::getProgramOptions().args =
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
            clEnumValN(util::OPT_Oz, "Oz", "Enable maximum size optimizations"),
            nullptr),
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
                              "Disable all log messages"),
                   nullptr),
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
    cl::list<std::string> inputFilesArg(cl::desc("Input file list"),
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
                       "Native object format '.o' (default)"),
            nullptr));
    // LLVM binary location
    cl::opt<std::string> llvmDirArg("llvm-dir",
                                    cl::desc("LLVM binary directory"),
                                    cl::init(""), cl::cat(catLLVM));
    // LLVM llvm-as name
    cl::opt<std::string> llvmAsArg("llvm-as", cl::desc("LLVM 'llvm-as' binary"),
                                   cl::init("llvm-as"), cl::cat(catLLVM));
    // LLVM llc name
    cl::opt<std::string> llvmLlcArg("llvm-llc", cl::desc("LLVM 'llc' binary"),
                                    cl::init("llc"), cl::cat(catLLVM));
    // LLVM opt name
    cl::opt<std::string> llvmOptArg("llvm-opt", cl::desc("LLVM 'opt' binary"),
                                    cl::init("opt"), cl::cat(catLLVM));
    // x86 asm syntax
    auto map = cl::getRegisteredOptions();
    map["x86-asm-syntax"]->setArgStr("llvm-x86-asm-syntax");
    cl::opt<util::X86AsmSyntax> x86AsmArg(
        "x86-asm-syntax", cl::desc("Emitted x86 assembly syntax"),
        cl::init(util::X86_ATT),
        cl::values(
            clEnumValN(util::X86_ATT, "att", "AT&T assembly syntax"),
            clEnumValN(util::X86_INTEL, "intel", "Intel assembly syntax"),
            nullptr),
        cl::cat(catCodegen));

    {
        auto arr = std::vector<const decltype(catGeneral)*>{
            &catGeneral, &catCodegen, &catLLVM};
        cl::HideUnrelatedOptions(arr);
    }
    cl::ParseCommandLineOptions(argc, argv, "Varuna Compiler");

    // Process arguments
    spdlog::set_level(logArg);

    util::getProgramOptions().optLevel = optArg;
    util::getProgramOptions().loggingLevel = logArg;

    if(licenseArg)
    {
        // License requested, show it and quit
        showLicense();
        return 0;
    }

    // Create Runner
    int threads = jobsArg;
    if(threads < 0)
    {
        util::logger->error("Invalid number of jobs: {}", threads);
        return -1;
    }
    Runner runner(threads);

    std::vector<std::string> filelist;
    if(inputFilesArg.empty())
    {
        util::logger->error("At least one input file required");
        return -1;
    }
    if(inputFilesArg.size() > 1)
    {
        // TODO: Multiple input files
        util::logger->error("Only one input file is supported");
        return -1;
    }
    for(auto&& f : inputFilesArg)
    {
        filelist.push_back(std::move(f));
    }

    util::getProgramOptions().inputFilenames = std::move(filelist);
    util::getProgramOptions().outputFilename = std::move(outputFileArg);
    util::getProgramOptions().output = std::move(outputArg);

    util::getProgramOptions().emitDebug = debugArg;

    util::getProgramOptions().llvmBinDir = std::move(llvmDirArg);
    util::getProgramOptions().llvmAsBin = std::move(llvmAsArg);
    util::getProgramOptions().llvmLlcBin = std::move(llvmLlcArg);
    util::getProgramOptions().llvmOptBin = std::move(llvmOptArg);

    util::getProgramOptions().x86asm = x86AsmArg;

    // Run it
    if(!runner.run())
    {
        util::logger->info("Compilation failed");
        return 1;
    }

    util::logger->info("Compilation successful");
    return 0;
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
