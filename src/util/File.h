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

    explicit File(std::string pFilename)
        : filename(std::move(pFilename)), checksum(0), contentValid(false)
    {
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    File(File&&) noexcept = default;
    File& operator=(File&&) noexcept = default;

    ~File() noexcept = default;

    /// Read file contents
    bool readFile();
    /// Check if utf8 in the file is valid
    bool checkUtf8();
    /// Calculates checksum of the file
    [[noreturn]] void calculateChecksum();

    const std::string& getFilename() const;
    const std::string& getContent() const;

    std::string&& consumeContent();

    void setContent(const std::string& c);
    void setContent(std::string&& c);

private:
    std::string _readFile(const std::string& fname);

    std::string filename, content;
    Checksum_t checksum;
    bool contentValid;
};

inline const std::string& File::getFilename() const
{
    return filename;
}
inline const std::string& File::getContent() const
{
    if(!contentValid)
    {
        throw std::runtime_error("Reading invalid file contents");
    }
    return content;
}

inline std::string&& File::consumeContent()
{
    if(!contentValid)
    {
        throw std::runtime_error("Consuming invalid file contents");
    }
    contentValid = false;
    return std::move(content);
}

inline void File::setContent(const std::string& c)
{
    if(contentValid)
    {
        throw std::runtime_error("Rewriting valid file contents");
    }
    content = c;
    contentValid = true;
}
inline void File::setContent(std::string&& c)
{
    if(contentValid)
    {
        throw std::runtime_error("Rewriting valid file contents");
    }
    content = std::move(c);
    contentValid = true;
}
} // namespace util
