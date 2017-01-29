// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "fe/cli/CLI.h"
#include "fe/app/App.h"
#include "util/MathUtils.h"
#include <llvm/Config/llvm-config.h>
#include <llvm/Support/CommandLine.h>

namespace fe
{
namespace cli
{
    static const std::string ver =
        fmt::format("{} (LLVM version: {}) Build date: {}",
                    util::programinfo::version::toString(), LLVM_VERSION_STRING,
                    util::programinfo::getBuildDate());

    CLI::CLI(int pArgc, char** pArgv) : argc(pArgc), argv(pArgv)
    {
    }

    int CLI::run()
    {
        using namespace llvm;

        cl::SetVersionPrinter(&showVersion);

        // Specify options
        cl::OptionCategory cat("Varuna compiler options");

        // Optimization level
        cl::opt<util::OptimizationLevel> optArg(
            cl::desc("Optimization level"), cl::init(util::OPT_O2),
            cl::values(
                clEnumValN(util::OPT_O0, "O0", "No optimizations"),
                clEnumValN(util::OPT_O1, "O1", "Enable trivial optimizations"),
                clEnumValN(util::OPT_O2, "O2", "Enable default optimizations"),
                clEnumValN(util::OPT_O3, "O3",
                           "Enable expensive optimizations"),
                clEnumValN(util::OPT_Os, "Os", "Enable size optimizations"),
                clEnumValN(util::OPT_Oz, "Oz",
                           "Enable maximum size optimizations"),
                nullptr),
            cl::cat(cat));
        // Logging level
        cl::opt<spdlog::level::level_enum> logArg(
            "logging", cl::desc("Logging level"), cl::init(spdlog::level::info),
            cl::values(
                clEnumValN(spdlog::level::trace,
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
            cl::cat(cat));
        // Jobs
        cl::opt<int> jobsArg(
            "j", cl::desc("Number of worker threads to use (Default: 1)"),
            cl::value_desc("threads"), cl::init(1), cl::cat(cat));
        // License
        cl::opt<bool> licenseArg(
            "license", cl::desc("Print license and copyright information"),
            cl::init(false), cl::cat(cat));
        // Output file
        cl::opt<std::string> outputFileArg("o", cl::desc("Output file"),
                                           cl::init("stdout"), cl::cat(cat));
        // Input files
        cl::list<std::string> inputFilesArg(cl::desc("Input file list"),
                                            cl::value_desc("file"),
                                            cl::Positional, cl::cat(cat));
        cl::opt<util::IntSize> intSizeArg(
            "fint-size", cl::desc("Size of type 'int' (Default: 0)"),
            cl::init(util::INTSIZE_VOIDPTR),
            cl::values(
                clEnumValN(util::INTSIZE_VOIDPTR, "0",
                           "32 or 64 bits depending on the CPU architecture"),
                clEnumValN(util::INTSIZE_32, "32", "32 bits"),
                clEnumValN(util::INTSIZE_64, "64", "64 bits"), nullptr),
            cl::cat(cat));

        cl::HideUnrelatedOptions(cat);
        cl::ParseCommandLineOptions(argc, argv, "Varuna Compiler");

        // Process arguments
        spdlog::set_level(logArg);

        util::getProgramOptions().optLevel = optArg;
        util::getProgramOptions().loggingLevel = logArg;

        if(licenseArg)
        {
            showLicense();
            return 0;
        }

        int threads = jobsArg;
        if(threads < 0)
        {
            util::logger->error("Invalid number of jobs: {}", threads);
            return -1;
        }
        auto app = std::make_unique<api::Application>(threads);

        std::vector<std::string> filelist;
        if(inputFilesArg.empty())
        {
            util::logger->error("At least one input file required");
            return -1;
        }
        if(inputFilesArg.size() > 1)
        {
            util::logger->error("Only one input file is supported");
            return -1;
        }
        for(auto&& f : inputFilesArg)
        {
            filelist.push_back(std::move(f));
        }
        util::getProgramOptions().inputFilenames = std::move(filelist);
        util::getProgramOptions().outputFilename = std::move(outputFileArg);

        int intSize = intSizeArg;
        if(intSize == 0)
        {
            intSize = sizeof(void*) * 8;
        }
        util::getProgramOptions().intSize = static_cast<size_t>(intSize);

        if(!app->execute())
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
} // namespace cli
} // namespace fe
