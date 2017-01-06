// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/Logger.h"
#include <string>

namespace util
{
class File
{
public:
    using Checksum_t = uint64_t;

    explicit File(std::string pFilename);

    bool readFile();
    bool checkUtf8();
    [[noreturn]] void calculateChecksum();

    std::string filename, content;
    Checksum_t checksum;

private:
    std::string _readFile(const std::string& fname);
};
} // namespace util
