// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace util
{
/// Source file
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

    File(File&&) = default;
    File& operator=(File&&) = default;

    ~File() noexcept = default;

    /**
     * Read file contents
     * @return Success
     */
    bool readFile();
    /**
     * Check for utf8 validity in the file
     * @return Success
     */
    bool checkUtf8();
    /**
     * Calculate the checksum of the file
     */
    [[noreturn]] void calculateChecksum();

    /**
     * Get filename
     * @return Filename
     */
    const std::string& getFilename() const;
    /**
     * Get contents
     * @return Contents
     */
    const std::string& getContent() const;

    /**
     * Get a line of contents.
     * Throws at invalid line number
     * @param  line Line number
     * @return      Line contents
     */
    const std::string& getLine(uint32_t line);

    /**
     * Set content of the file
     * @param c File contents
     */
    void setContent(const std::string& c);
    void setContent(std::string&& c);

private:
    /// Actually read the file
    std::string _readFile(const std::string& fname);

    /// Filename
    std::string filename;
    /// File contents
    std::string content;
    /// Rows of the file
    std::vector<std::string> contentRows;
    /// File checksum
    Checksum_t checksum;
    /// Are file contents usable
    bool contentValid;
};

inline const std::string& File::getFilename() const
{
    return filename;
}
inline const std::string& File::getContent() const
{
    assert(contentValid);
    return content;
}

inline void File::setContent(const std::string& c)
{
    assert(!contentValid);
    content = c;
    contentValid = true;
}
inline void File::setContent(std::string&& c)
{
    assert(!contentValid);
    content = std::move(c);
    contentValid = true;
}
} // namespace util
