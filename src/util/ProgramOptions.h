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
    EMIT_NONE = 0,
    EMIT_AST = 1 << 0,
    EMIT_LLVM_IR = 1 << 1,
    EMIT_LLVM_BC = 1 << 2,
    EMIT_ASM = 1 << 3,
    EMIT_OBJ = 1 << 4
};

enum X86AsmSyntax
{
    X86_ATT,
    X86_INTEL
};

/// Program options
struct ProgramOptions
{
    /// Input file list
    std::vector<std::string> inputFilenames{};
    /// Output filename
    std::string outputFilename{"-"};
    /// Logging level
    spdlog::level::level_enum loggingLevel{spdlog::level::info};
    /// Optimization level
    OptimizationLevel optLevel{OPT_O2};
    /// Emit debugging symbols
    bool emitDebug{false};
    /// Output
    OutputType output{EMIT_OBJ};
    /// Argument string
    std::string args{""};
    /// LLVM binary directory
    std::string llvmBinDir{""};
    /// llvm-as binary
    std::string llvmAsBin{""};
    /// llc binary
    std::string llvmLlcBin{""};
    /// opt binary
    std::string llvmOptBin{""};
    /// x86 asm syntax
    X86AsmSyntax x86asm{X86_ATT};

    /**
     * Get speed and size optimization levels from optLevel
     * @return Speed and size levels
     */
    std::pair<uint8_t, uint8_t> getOptLevel() const
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
            o = 2;
            s = 1;
            break;
        case OPT_Oz:
            o = 2;
            s = 2;
            break;
        }

        return {o, s};
    }

    std::string optLevelToString() const
    {
        uint8_t o, s;
        std::tie(o, s) = getOptLevel();
        if(o == 0 && s == 0)
        {
            return "-O0";
        }
        if(s == 0)
        {
            return fmt::format("-O{}", o);
        }
        if(s == 1)
        {
            return "-Os";
        }
        return "-Oz";
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
