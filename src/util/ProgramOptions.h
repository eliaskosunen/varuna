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
/// Optimization level
enum OptimizationLevel
{
    OPT_O0 = 0, ///< -O0
    OPT_O1,     ///< -O1
    OPT_O2,     ///< -O2
    OPT_O3,     ///< -O3
    OPT_Os,     ///< -Os
    OPT_Oz      ///< -Oz
    // OPT_Omax    ///< -O3 + lto
};

enum OutputType
{
    EMIT_AST,
    EMIT_LLVM_IR,
    EMIT_LLVM_BC,
    EMIT_ASM,
    EMIT_OBJ
};

/// Program options
struct ProgramOptions
{
    /// Input file list
    std::vector<std::string> inputFilenames{};
    /// Output filename
    std::string outputFilename{""};
    /// Logging level
    spdlog::level::level_enum loggingLevel{spdlog::level::info};
    /// Optimization level
    OptimizationLevel optLevel{OPT_O2};
    /// Emit debugging symbols
    bool emitDebug{false};
    /// Output
    OutputType output{EMIT_LLVM_IR};

    /**
     * Get speed and size optimization levels from optLevel
     * @return Speed and size levels
     */
    std::pair<uint8_t, uint8_t> getOptLevel() const noexcept
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

        return {o, s};
    }
};

/// Get mutable reference to global ProgramOptions.
/// Only use before spawning any threads to avoid baaad things
inline ProgramOptions& getProgramOptions() noexcept
{
    static ProgramOptions p;
    return p;
}

/// Get immutable reference to global ProgramOptions
inline const ProgramOptions& viewProgramOptions() noexcept
{
    return getProgramOptions();
}
}
