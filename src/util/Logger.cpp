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
// Create loggers
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

    // Set logger patterns
    // Varuna [name] level: message
    spdlog::set_pattern(std::string(programinfo::name) + " [%n] %l: %v");
    // Varuna level: message
    logger->set_pattern(std::string(programinfo::name) + " %l: %v");
    // Message
    loggerBasic->set_pattern("%v");

    // Only flush on errors
    logger->flush_on(spdlog::level::err);
    loggerBasic->flush_on(spdlog::level::err);
}

void dropLogger()
{
    spdlog::drop_all();
}

std::string formatCompilerMessage(SourceLocation loc)
{
    // s for std::string
    using namespace std::string_literals;
    // _a for fmt::arg
    using namespace fmt::literals;

    // Get error and previous line content
    const auto& lineContent = loc.file->getLine(loc.line);
    const auto& prevLineContent = [&]() {
        if(loc.line == 1)
        {
            return ""s;
        }
        return loc.file->getLine(loc.line - 1);
    }();

    // Create arrow
    // If error is in the column 5 or more,
    // the arrow comes from the left
    const auto isArrowLeft = loc.col >= 5;
    const auto arrow = [&]() {
        if(isArrowLeft)
        {
            return "---^"s;
        }
        return "^---"s;
    }();
    // Calculate the arrow left padding
    const auto arrowPaddingLen = [&]() {
        if(isArrowLeft)
        {
            // If arrow comes from the left,
            // the padding is column - (arrow length - 1)
            // -1 because the arrow pointing upwards is counted in the arrow
            // length
            return loc.col - 3;
        }
        // If the arrow comes from the left,
        // the padding is column - 1
        // -1 because of the arrow pointing upwards
        return loc.col - 1;
    }();
    const auto arrowPadding =
        stringutils::createEmptyStringWithLength(arrowPaddingLen);

    // Count the number of digits in the line number
    const auto lineNumberLen = numDigits(loc.line);
    // Create the right amount of padding for the left side of the pipe '|'
    const auto padding =
        stringutils::createEmptyStringWithLength(lineNumberLen);

    // Finally format it
    return fmt::format(" {padding} | {prevLine}\n"
                       " {line} | {content}\n"
                       " {padding} | {arrowPadding}{arrow}",
                       "padding"_a = padding, "prevLine"_a = prevLineContent,
                       "line"_a = loc.line, "content"_a = lineContent,
                       "arrowPadding"_a = arrowPadding, "arrow"_a = arrow);
}
} // namespace util
