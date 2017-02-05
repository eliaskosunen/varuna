// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <spdlog.h>
#include <string>
#include <tuple>
#include <vector>

namespace util
{
enum OptimizationLevel
{
    OPT_O0 = 0,
    OPT_O1,
    OPT_O2,
    OPT_O3,
    // OPT_Omax, // O3 + lto
    OPT_Os,
    OPT_Oz
};
enum IntSize
{
    INTSIZE_VOIDPTR = 0,
    INTSIZE_32 = 32,
    INTSIZE_64 = 64
};

struct ProgramOptions
{
    std::vector<std::string> inputFilenames{};
    std::string outputFilename{""};
    spdlog::level::level_enum loggingLevel{spdlog::level::info};
    OptimizationLevel optLevel{OPT_O2};
    size_t intSize{0};
    bool emitDebug{false};

    auto getOptLevel() const noexcept
    {
        uint8_t o = 0;
        uint8_t s = 0;
        switch(optLevel)
        {
        case OPT_O0:
            break;
        case OPT_O1:
            o = 1;
            break;
        case OPT_O2:
            o = 2;
            break;
        case OPT_O3:
            o = 3;
            break;
        case OPT_Os:
            s = 1;
            break;
        case OPT_Oz:
            s = 2;
            break;
        }

        return std::make_tuple(o, s);
    }
};

inline ProgramOptions& getProgramOptions() noexcept
{
    static ProgramOptions p;
    return p;
}

inline const ProgramOptions& viewProgramOptions() noexcept
{
    return getProgramOptions();
}
}
