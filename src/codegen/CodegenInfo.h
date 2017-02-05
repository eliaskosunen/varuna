// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include <memory>

namespace codegen
{
/// Information for Codegen, CodegenVisitor and Optimizer
struct CodegenInfo
{
    /// Are optimizations enabled
    bool optEnabled() const
    {
        return optLevel > 0 || sizeLevel > 0;
    }

    std::shared_ptr<util::File> file;
    uint8_t optLevel{0};
    uint8_t sizeLevel{0};
    bool emitDebug{false};
};
} // namespace codegen
