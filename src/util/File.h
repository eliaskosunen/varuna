// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <cereal.h>
#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace util
{
/// Varuna source file
class File
{
public:
    using Checksum_t = uint64_t;

    File() = default;
    explicit File(std::string pFilename) : filename(std::move(pFilename))
    {
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    File(File&&) noexcept = default;
    File& operator=(File&&) = default;

    ~File() noexcept = default;

    /**
     * Read file contents into class buffer.
     * Get file contents with getContent() or consumeContent().
     * \return Success
     */
    bool readFile();

    /**
     * Get filename
     * \return Filename
     */
    const std::string& getFilename() const;
    /**
     * Get contents
     * \return Contents
     */
    const std::string& getContent() const;

    /**
     * Get ownership of the file contents.
     * Invalidates file contents inside this class.
     * \return Contents
     */
    std::string&& consumeContent();

    /// Set contents inside this class
    void setContent(std::string c);

    /// Are file contents valid
    bool isValid() const
    {
        return contentValid;
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(filename));
    }

private:
    /**
     * Actually read the file.
     * Called by readFile()
     * \throw std::runtime_error On error
     * \param  fname Filename to read
     * \return       File contents
     */
    std::string _readFile(const std::string& fname);

    /// Filename
    std::string filename{"[undefined]"};
    /// File contents
    std::string content{""};
    /// File checksum
    Checksum_t checksum{0};
    /// Are file contents usable
    bool contentValid{false};
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

inline std::string&& File::consumeContent()
{
    assert(contentValid);
    return std::move(content);
}

inline void File::setContent(std::string c)
{
    assert(!contentValid);
    content = std::move(c);
    contentValid = true;
}
} // namespace util
