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
    std::string inputFilename{""};
    /// Output filename
    std::string outputFilename{"-"};
    /// Logging level
    spdlog::level::level_enum loggingLevel{spdlog::level::info};
    /// Optimization level
    OptimizationLevel optLevel{OPT_O0};
    /// Emit debugging symbols
    bool emitDebug{false};
    /// Output
    OutputType output{EMIT_OBJ};
    /// Argument string
    std::string args{""};
    /// x86 asm syntax
    X86AsmSyntax x86asm{X86_ATT};
    /// Generate module file
    bool generateModuleFile{true};
    /// Strip debug info
    bool stripDebug{false};
    /// Strip source filename
    bool stripSourceFilename{false};

    /**
     * Get speed and size optimization levels from optLevel
     * @return Speed and size levels
     */
    std::pair<uint8_t, uint8_t> getOptLevel() const;

    std::string optLevelToString() const;

    /// Get mutable reference to global ProgramOptions.
    /// Only use before spawning any threads to avoid baaad things
    static ProgramOptions& get() noexcept
    {
        return instance;
    }

    /// Get immutable reference to global ProgramOptions
    static const ProgramOptions& view() noexcept
    {
        return instance;
    }

private:
    static ProgramOptions instance;
};
} // namespace util
