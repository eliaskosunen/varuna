// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <guid.h>
#include <string>

namespace util
{
class TmpFile
{
    static GuidGenerator g;

public:
    TmpFile(const std::string& prefix, const std::string& suffix);
    ~TmpFile();

    TmpFile(const TmpFile&) = delete;
    TmpFile& operator=(const TmpFile&) = delete;

    TmpFile(TmpFile&&) = default;
    TmpFile& operator=(TmpFile&&) = default;

    const std::string& getFilename() const;

private:
    std::string name;
};
} // namespace util
