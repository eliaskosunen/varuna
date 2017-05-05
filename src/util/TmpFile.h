// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <guid.h>
#include <string>

namespace util
{
/// Temporary file for e.g. IR output
class TmpFile
{
    static GuidGenerator g;

public:
    /// Filename format: prefix-randomguid.suffix
    /// \throw std::runtime_error On failure
    TmpFile(const std::string& prefix, const std::string& suffix);
    ~TmpFile();

    TmpFile(const TmpFile&) = delete;
    TmpFile& operator=(const TmpFile&) = delete;

    TmpFile(TmpFile&&) noexcept = default;
    TmpFile& operator=(TmpFile&&) = default;

    /// Get the filename of the temporary file
    const std::string& getFilename() const;

private:
    std::string name;
};
} // namespace util
