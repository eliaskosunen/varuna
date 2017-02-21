// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/File.h"
#include "util/Logger.h"
#include "util/Process.h"
#include "util/ProgramInfo.h"
#include "util/StringUtils.h"
#include <doctest.h>

static void compare(const std::string& lhs, const std::string& rhs)
{
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
                const std::string& flags = "")
{
    auto p = util::Process(
        fmt::format("{}/bin/varuna", dir()),
        fmt::format(
            "-no-module -strip-debug -strip-source-filename "
            "-logging=warning -llvm-as=llvm-as-3.9 "
            "-llvm-llc=llc-3.9{} -llvm-opt=opt-3.9 {}/src/tests/inputs/{} "
            "-o {}/src/tests/outputs/{}",
            flags.empty() ? "" : (" " + flags), dir(), inputFilename, dir(),
            outputFilename));
    CHECK(p.spawn());
    CHECK(p.getReturnValue() == 0);
    REQUIRE(p.getErrorString() == "");

    util::File output(
        fmt::format("{}/src/tests/outputs/{}", dir(), outputFilename));
    CHECK(output.readFile());

    util::File refOutput(
        fmt::format("{}/src/tests/ref_outputs/{}", dir(), outputFilename));
    CHECK(refOutput.readFile());

    compare(output.getContent(), refOutput.getContent());
}

static void runEmitLLVM(const std::string& inputFilename,
                        const std::string& outputFilename,
                        const std::string& flags = "")
{
    run(inputFilename, outputFilename,
        fmt::format("-emit=llvm-ir{}", flags.empty() ? "" : (" " + flags)));
}

TEST_SUITE("System tests");

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

TEST_SUITE_END();

TEST_SUITE("System tests with expected errors");

static void runExpectError(const std::string& inputFilename)
{
    auto p = util::Process(
        fmt::format("{}/bin/varuna", dir()),
        fmt::format("-no-module -strip-debug -strip-source-filename "
                    "-logging=critical -llvm-as=llvm-as-3.9 "
                    "-llvm-llc=llc-3.9 -llvm-opt=opt-3.9 "
                    "{}/src/tests/error_inputs/{} "
                    "-o - -emit=none",
                    dir(), inputFilename));
    CHECK(p.spawn());
    CHECK(p.getReturnValue() != 0);
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

TEST_SUITE_END();
