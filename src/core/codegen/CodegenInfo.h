// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <string>

namespace core
{
namespace codegen
{
    struct CodegenInfo
    {
        bool optEnabled() const
        {
            return optLevel > 0 || sizeLevel > 0;
        }

        const std::string& filename;
        uint8_t optLevel{0};
        uint8_t sizeLevel{0};
    };
} // namespace codegen
} // namespace core
