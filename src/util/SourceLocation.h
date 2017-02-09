// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include <spdlog.h>

namespace util
{
struct SourceLocation
{
    SourceLocation() = default;
    SourceLocation(std::shared_ptr<util::File> f, uint32_t l, uint32_t c)
        : file(f), line(l), col(c)
    {
    }

    /// Format to a string
    std::string toString() const
    {
        return fmt::format("{}:{}:{}", file->getFilename(), line, col);
    }

    /// File
    std::shared_ptr<util::File> file;
    /// Line
    uint32_t line{0};
    /// Column
    uint32_t col{0};
};
}
