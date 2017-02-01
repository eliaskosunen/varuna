// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <istream>
#include <memory>
#include <stdexcept>

namespace util
{
/**
 * Read an istream
 */
class StreamReader
{
public:
    /**
     * Read a file
     * @param  filename Name of the file to read
     * @return          Contents of the file
     */
    static std::string readFile(const std::string& filename);
};
} // namespace util
