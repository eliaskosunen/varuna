// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/Logger.h"
#include "util/MathUtils.h"
#include "util/ProgramInfo.h"
#include "util/StringUtils.h"
#include <string>

namespace util
{
std::shared_ptr<spdlog::logger> logger = createLogger(true, "console_color");
std::shared_ptr<spdlog::logger> loggerBasic =
    createLogger(false, "console_nocolor");

void initLogger()
{
    static bool set = false;
    if(set)
    {
        return;
    }

    spdlog::set_pattern(std::string(programinfo::name) + " [%n] %l: %v");
    logger->set_pattern(std::string(programinfo::name) + " %l: %v");
    loggerBasic->set_pattern("%v");

    logger->flush_on(spdlog::level::err);
    loggerBasic->flush_on(spdlog::level::err);
}

void dropLogger()
{
    spdlog::drop_all();
}

std::string formatCompilerMessage(File* file, SourceLocation loc)
{
    using namespace std::string_literals;
    using namespace fmt::literals;

    const auto& lineContent = file->getLine(loc.line);
    const auto& prevLineContent = [&]() {
        if(loc.line == 1)
        {
            return ""s;
        }
        return file->getLine(loc.line - 1);
    }();

    const auto isArrowLeft = loc.col >= 5;
    const auto arrow = [&]() {
        if(isArrowLeft)
        {
            return "---^"s;
        }
        return "^---"s;
    }();
    const auto arrowPaddingLen = [&]() {
        if(isArrowLeft)
        {
            return loc.col - 4;
        }
        return loc.col - 1;
    }();
    const auto arrowPadding =
        StringUtils::createEmptyStringWithLength(arrowPaddingLen);

    const auto lineNumberLen = numDigits(loc.line);
    const auto padding =
        StringUtils::createEmptyStringWithLength(lineNumberLen);

    return fmt::format(" {padding} | {prevLine}\n"
                       " {line} | {content}\n"
                       " {padding} | {arrowPadding}{arrow}",
                       "padding"_a = padding, "prevLine"_a = prevLineContent,
                       "line"_a = loc.line, "content"_a = lineContent,
                       "arrowPadding"_a = arrowPadding, "arrow"_a = arrow);
}
} // namespace util
