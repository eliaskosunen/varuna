// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/File.h"
#include "util/Logger.h"
#include "util/Process.h"
#include "util/ProgramInfo.h"
#include "util/StringUtils.h"
#include <doctest.h>
#include <cstdlib>

using namespace fmt::literals;

static void compare(std::string&& lhs, std::string&& rhs,
                    bool skipFirstLine = false)
{
    if(skipFirstLine)
    {
        auto skip = [&](std::string&& str) {
            auto it = str.begin();
            while(it != str.end() && *it != '\n')
            {
                ++it;
            }
            return std::string(it, str.end());
        };
        lhs = skip(std::move(lhs));
        rhs = skip(std::move(rhs));
    }

    if(lhs.empty() || rhs.empty())
    {
        throw std::runtime_error("Compared strings can't be empty");
    }

    const auto lhsLen = lhs.length();
    const auto rhsLen = rhs.length();
    for(size_t i = 0; i < lhsLen; ++i)
    {
        if(lhs[i] == rhs[i])
        {
            continue;
        }
        if(lhsLen == rhsLen && i + 2 != lhsLen)
        {
            continue;
        }

        auto begin = [&]() -> std::ptrdiff_t {
            if(i <= 25)
            {
                return 0;
            }
            return static_cast<std::ptrdiff_t>(i - 25);
        }();
        auto end = static_cast<std::ptrdiff_t>(i);

        auto lhsSnippet = std::string(lhs.begin() + begin, lhs.begin() + end);
        auto rhsSnippet = std::string(rhs.begin() + begin, rhs.begin() + end);
        CHECK(lhsSnippet == rhsSnippet);
        if(lhsSnippet != rhsSnippet)
        {
            break;
        }

        i += static_cast<size_t>(end - begin + 1);
    }
}

static auto& dir()
{
    static const auto d = [&]() {
        auto dirparts =
            util::stringutils::split(util::getCurrentDirectory(), '/');
        auto dirpartsNew = dirparts;
        for(auto it = dirparts.rbegin(); it != dirparts.rend(); ++it)
        {
            if(*it == "varuna")
            {
                break;
            }
            dirpartsNew.push_back("..");
        }
        return util::stringutils::join(dirpartsNew, '/');
    }();
    return d;
}

static void run(const std::string& inputFilename,
                const std::string& outputFilename,
                const std::string& flags = "", bool createModuleFile = false)
{
    auto p = util::Process(
        fmt::format("{dir}/bin/varuna", "dir"_a = dir()),
        fmt::format("{module}-strip-debug -strip-source-filename "
                    "-logging=warning -llvm-as=llvm-as-3.9 "
                    "-llvm-llc=llc-3.9{flags} -llvm-opt=opt-3.9 "
                    "{dir}/src/tests/inputs/{in} "
                    "-o {dir}/src/tests/outputs/{out}",
                    "dir"_a = dir(),
                    "module"_a = (createModuleFile ? "" : "-no-module "),
                    "flags"_a = (flags.empty() ? "" : (" " + flags)),
                    "in"_a = inputFilename, "out"_a = outputFilename));
    CHECK(p.spawn());
    CHECK(p.getReturnValue() == 0);
    REQUIRE(p.getErrorString() == util::Process::getSuccessErrorString());

    util::File output(fmt::format("{dir}/src/tests/outputs/{out}",
                                  "dir"_a = dir(), "out"_a = outputFilename));
    REQUIRE(output.readFile());

    util::File refOutput(fmt::format("{dir}/src/tests/ref_outputs/{out}",
                                     "dir"_a = dir(),
                                     "out"_a = outputFilename));
    REQUIRE(refOutput.readFile());

    compare(output.consumeContent(), refOutput.consumeContent());
}

static void runEmitLLVM(const std::string& inputFilename,
                        const std::string& outputFilename,
                        const std::string& flags = "",
                        bool createModuleFile = false)
{
    run(inputFilename, outputFilename,
        fmt::format("-emit=llvm-ir{flags}",
                    "flags"_a = (flags.empty() ? "" : (" " + flags))),
        createModuleFile);
}

static void runEmitLLVMWithModules(const std::string& inputFilename,
                                   const std::string& outputFilename,
                                   const std::string& moduleName,
                                   const std::string& flags = "")
{
    runEmitLLVM(inputFilename, outputFilename, flags, true);

#if 0
    util::File modOutput(
        fmt::format("{}/src/tests/outputs/{}", dir(), moduleName));
    REQUIRE(modOutput.readFile());

    util::File modRefOutput(
        fmt::format("{}/src/tests/ref_outputs/{}", dir(), moduleName));
    REQUIRE(modRefOutput.readFile());

    compare(modOutput.consumeContent(), modRefOutput.consumeContent());
#endif
}

TEST_SUITE("System tests");

TEST_CASE("Emit AST")
{
    auto p = util::Process(
        fmt::format("{dir}/bin/varuna", "dir"_a = dir()),
        fmt::format("-no-module -strip-debug -strip-source-filename "
                    "-logging=error -llvm-as=llvm-as-3.9 "
                    "-llvm-llc=llc-3.9 -llvm-opt=opt-3.9 "
                    "{dir}/src/tests/inputs/02_main.va "
                    "-o - -emit=ast",
                    "dir"_a = dir()));
    CHECK(p.spawn());
    CHECK(p.getReturnValue() == 0);
    REQUIRE(p.getErrorString() == util::Process::getSuccessErrorString());
}

TEST_CASE("Emit assembly")
{
    auto p = util::Process(
        fmt::format("{dir}/bin/varuna", "dir"_a = dir()),
        fmt::format("-no-module -strip-debug -strip-source-filename -O3 "
                    "-logging=error -llvm-as=llvm-as-3.9 "
                    "-llvm-llc=llc-3.9 -llvm-opt=opt-3.9 "
                    "{dir}/src/tests/inputs/02_main.va "
                    "-o {dir}/src/tests/outputs/02_main_asm.s -emit=asm",
                    "dir"_a = dir()));
    CHECK(p.spawn());
    CHECK(p.getReturnValue() == 0);
    REQUIRE(p.getErrorString() == util::Process::getSuccessErrorString());
}

TEST_CASE("Emit object code")
{
    auto p = util::Process(
        fmt::format("{dir}/bin/varuna", "dir"_a = dir()),
        fmt::format("-no-module -strip-debug -strip-source-filename -O3 "
                    "-logging=error -llvm-as=llvm-as-3.9 "
                    "-llvm-llc=llc-3.9 -llvm-opt=opt-3.9 "
                    "{dir}/src/tests/inputs/02_main.va "
                    "-o {dir}/src/tests/outputs/02_main_obj.o -emit=obj",
                    "dir"_a = dir()));
    CHECK(p.spawn());
    CHECK(p.getReturnValue() == 0);
    REQUIRE(p.getErrorString() == util::Process::getSuccessErrorString());
}

TEST_CASE("Emit LLVM BC")
{
    auto p = util::Process(
        fmt::format("{dir}/bin/varuna", "dir"_a = dir()),
        fmt::format(
            "-no-module -strip-debug -strip-source-filename -O3 "
            "-logging=error -llvm-as=llvm-as-3.9 "
            "-llvm-llc=llc-3.9 -llvm-opt=opt-3.9 "
            "{dir}/src/tests/inputs/02_main.va "
            "-o {dir}/src/tests/outputs/02_main_bctest.bc -emit=llvm-bc",
            "dir"_a = dir()));
    CHECK(p.spawn());
    CHECK(p.getReturnValue() == 0);
    REQUIRE(p.getErrorString() == util::Process::getSuccessErrorString());

    {
        auto llvmDisProc = util::Process(
            "llvm-dis-3.9",
            fmt::format("{dir}/src/tests/outputs/02_main_bctest.bc -o "
                        "{dir}/src/tests/outputs/02_main_bctest.ll",
                        "dir"_a = dir()));
        CHECK(llvmDisProc.spawn());
        CHECK(llvmDisProc.getReturnValue() == 0);
        REQUIRE(llvmDisProc.getErrorString() ==
                util::Process::getSuccessErrorString());
    }

    util::File output(fmt::format("{dir}/src/tests/outputs/02_main_bctest.ll",
                                  "dir"_a = dir()));
    REQUIRE(output.readFile());

    util::File refOutput(
        fmt::format("{dir}/src/tests/ref_outputs/02_main.ll", "dir"_a = dir()));
    REQUIRE(refOutput.readFile());

    compare(output.consumeContent(), refOutput.consumeContent(), true);
}

TEST_CASE("01_empty")
{
    runEmitLLVM("01_empty.va", "01_empty.ll", "-O3");
}

TEST_CASE("02_main")
{
    runEmitLLVM("02_main.va", "02_main.ll", "-O3");
}

TEST_CASE("03_functions")
{
    runEmitLLVM("03_functions.va", "03_functions.ll", "-O0");
    runEmitLLVM("03_functions.va", "03_functions_opt.ll", "-O3");
}

TEST_CASE("04_arithmetic")
{
    runEmitLLVM("04_arithmetic.va", "04_arithmetic.ll", "-O0");
    runEmitLLVM("04_arithmetic.va", "04_arithmetic_opt.ll", "-O3");
}

TEST_CASE("05_variables")
{
    runEmitLLVM("05_variables.va", "05_variables.ll", "-O0");
    runEmitLLVM("05_variables.va", "05_variables_opt.ll", "-O3");
}

TEST_CASE("06_if")
{
    runEmitLLVM("06_if.va", "06_if.ll", "-O0");
    runEmitLLVM("06_if.va", "06_if_opt.ll", "-O3");
}

TEST_CASE("07_while")
{
    runEmitLLVM("07_while.va", "07_while.ll", "-O0");
    runEmitLLVM("07_while.va", "07_while_opt.ll", "-O3");
}

TEST_CASE("08_for")
{
    runEmitLLVM("08_for.va", "08_for.ll", "-O0");
    runEmitLLVM("08_for.va", "08_for_opt.ll", "-O3");
}

TEST_CASE("09_cast")
{
    runEmitLLVM("09_cast.va", "09_cast.ll", "-O0");
    runEmitLLVM("09_cast.va", "09_cast_opt.ll", "-O3");
}

TEST_CASE("10_strings")
{
    runEmitLLVM("10_strings.va", "10_strings.ll", "-O0");
    runEmitLLVM("10_strings.va", "10_strings_opt.ll", "-O3");
}

TEST_CASE("11_alias")
{
    runEmitLLVM("11_alias.va", "11_alias.ll", "-O0");
    runEmitLLVM("11_alias.va", "11_alias_opt.ll", "-O3");
}

TEST_CASE("12_char")
{
    runEmitLLVM("12_char.va", "12_char.ll", "-O0");
    runEmitLLVM("12_char.va", "12_char_opt.ll", "-O3");
}

TEST_CASE("13_globals")
{
    runEmitLLVM("13_globals.va", "13_globals.ll", "-O0");
    runEmitLLVM("13_globals.va", "13_globals_opt.ll", "-O3");
}

TEST_CASE("14_modules")
{
    runEmitLLVMWithModules("14_modules_importee.va", "14_modules_importee.ll",
                           "test_14_modules_importee.vamod", "-O0");
    runEmitLLVMWithModules("14_modules_importee.va",
                           "14_modules_importee_opt.ll",
                           "test_14_modules_importee.vamod", "-O3");
    runEmitLLVMWithModules("14_modules.va", "14_modules.ll",
                           "test_14_modules.vamod", "-O0");
    runEmitLLVMWithModules("14_modules.va", "14_modules_opt.ll",
                           "test_14_modules.vamod", "-O3");
}

TEST_SUITE_END();

TEST_SUITE("System tests with expected errors");

static void runExpectError(const std::string& inputFilename,
                           bool expectException = false)
{
    auto p = util::Process(
        fmt::format("{dir}/bin/varuna", "dir"_a = dir()),
        fmt::format("-no-module -strip-debug -strip-source-filename "
                    "-logging={log} -llvm-as=llvm-as-3.9 "
                    "-llvm-llc=llc-3.9 -llvm-opt=opt-3.9 "
                    "{dir}/src/tests/error_inputs/{in} "
                    "-o - -emit=none",
                    "dir"_a = dir(), "in"_a = inputFilename,
                    "log"_a = (expectException ? "off" : "critical")));
    REQUIRE(p.spawn());
    CHECK_FALSE(p.getReturnValue() == 0);
}

TEST_CASE("01_invalid_main")
{
    runExpectError("01_invalid_main.va");
}

TEST_CASE("02_invalid_token")
{
    runExpectError("02_invalid_token.va");
}

TEST_CASE("03_parser_error")
{
    runExpectError("03_parser_error.va");
}

TEST_CASE("04_type_mismatch")
{
    runExpectError("04_type_mismatch.va");
}

TEST_CASE("05_invalid_operation")
{
    runExpectError("05_invalid_operation.va");
}

TEST_CASE("06_invalid_cast")
{
    runExpectError("06_invalid_cast.va");
}

TEST_CASE("07_void_variable")
{
    runExpectError("07_void_variable.va");
}

TEST_CASE("08_invalid_variable_name")
{
    runExpectError("08_invalid_variable_name.va");
}

TEST_CASE("09_invalid_import")
{
    runExpectError("09_invalid_import.va");
}

TEST_SUITE_END();
