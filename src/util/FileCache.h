// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include "util/Logger.h"
#include <stdexcept>
#include <unordered_map>

namespace util
{
/// Storage of source files
class FileCache
{
public:
    FileCache() = default;

    /// Get the entire cache
    auto& getCache()
    {
        return cache;
    }
    const auto& getCache() const
    {
        return cache;
    }

    /**
     * Get a file by filename
     * \param  name Filename
     * \param  add  Add file first
     * \return      Shared pointer to file, nullptr if not found
     */
    std::shared_ptr<File> getFile(const std::string& name, bool add = false);
    /**
     * Get the names of all files
     * \return Name list
     */
    std::vector<std::string> getFilenameList() const;

    /**
     * Get all files
     * \return File list
     */
    std::vector<std::shared_ptr<File>> getFiles() const;

    /**
     * Get list of files by filename
     * \throw std::invalid_argument If filename given isn't in the cache
     * \param names Vector of filenames
     * \return File list
     */
    template <typename T>
    std::vector<std::shared_ptr<File>> getFilesByNames(const T& names) const;

    /**
     * Add a file to the cache
     * \throw  std::invalid_argument If file is nullptr
     * \param  file File
     * \param  read Read the file
     * \return      Success
     */
    bool addFile(std::shared_ptr<File> file, bool read = true);
    /**
     * Add a file to the cache
     * \param  name Filename
     * \param  read Read the file
     * \return      Success
     */
    bool addFile(const std::string& name, bool read = true);

private:
    /// The cache itself
    std::unordered_map<std::string, std::shared_ptr<File>> cache;
};

template <typename T>
inline std::vector<std::shared_ptr<File>>
FileCache::getFilesByNames(const T& names) const
{
    std::vector<std::shared_ptr<File>> files;
    files.reserve(names.size());

    for(const auto& fname : names)
    {
        auto f = getFile(fname);
        if(!f)
        {
            throw std::invalid_argument(
                fmt::format("File '{}' not found from cache", fname));
        }
        files.push_back(f);
    }
    return files;
}
} // namespace util
