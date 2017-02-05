// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <spdlog.h>

namespace util
{
struct SourceLocation
{
    SourceLocation() = default;
    SourceLocation(std::string f, uint32_t l, uint32_t c)
        : file(std::move(f)), line(l), col(c)
    {
    }

    std::string toString() const
    {
        return fmt::format("{}:{}:{}", file, line, col);
    }

    std::string file{"undefined"};
    uint32_t line{0};
    uint32_t col{0};
};
}
