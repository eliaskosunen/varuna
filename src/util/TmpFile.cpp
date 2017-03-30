// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/TmpFile.h"
#include "util/Platform.h"
#include <fmt.h>

namespace util
{
GuidGenerator TmpFile::g;

TmpFile::TmpFile(const std::string& prefix, const std::string& suffix)
{
    name = fmt::format("{}-{}.{}", prefix, g.newGuid(), suffix);
    auto f = std::fopen(name.c_str(), "w");
    if(!f)
    {
        char* errstr = std::strerror(errno);
        if(!errstr)
        {
            throw std::runtime_error(fmt::format(
                "Unable to create temporary file: Unknown errno: {}", errno));
        }
        throw std::runtime_error(
            fmt::format("Unable to create temporary file: {}", errstr));
    }
    std::fclose(f);
}

TmpFile::~TmpFile()
{
    std::remove(name.c_str());
}

const std::string& TmpFile::getFilename() const
{
    return name;
}

} // namespace util
