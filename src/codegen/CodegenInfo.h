// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include <memory>

namespace codegen
{
/// Information for Codegen and CodegenVisitor
struct CodegenInfo
{
    CodegenInfo() = default;
    CodegenInfo(std::shared_ptr<util::File> pFile, uint8_t pOptLevel,
                uint8_t pSizeLevel, bool pEmitDebug)
        : file(std::move(pFile)), optLevel(pOptLevel), sizeLevel(pSizeLevel),
          emitDebug(pEmitDebug)
    {
    }

    /**
     * Area any optimizations enabled
     * \return Enabled
     */
    bool optEnabled() const
    {
        return optLevel > 0 || sizeLevel > 0;
    }

    /// File to generate code for
    std::shared_ptr<util::File> file{nullptr};
    /// Optimization level (-O1, -O2, -O3)
    uint8_t optLevel{0};
    /// Size optimization level (-Os, -Oz)
    uint8_t sizeLevel{0};
    /// Will debugging symbols be emitted (-g)
    bool emitDebug{false};
};
} // namespace codegen
