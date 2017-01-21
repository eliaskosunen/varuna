// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include "util/Logger.h"
#include "util/StreamReader.h"
#include <utf8.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace util
{
class FileCache
{
public:
    FileCache()
    {
    }

    auto& getCache()
    {
        return cache;
    }
    const auto& getCache() const
    {
        return cache;
    }

    std::shared_ptr<File> getFile(const std::string& name) const;
    std::vector<std::string> getFilenameList() const;

    std::vector<std::shared_ptr<File>> getFiles() const;

    std::vector<std::shared_ptr<File>>
    getFilesByNames(const std::vector<std::string>& names) const;

    bool addFile(std::shared_ptr<File> file, bool read = true);
    bool addFile(const std::string& name, bool read = true);

private:
    std::unordered_map<std::string, std::shared_ptr<File>> cache;
};

inline std::shared_ptr<File> FileCache::getFile(const std::string& name) const
{
    auto fileit = cache.find(name);
    if(fileit == cache.end())
    {
        return nullptr;
    }
    return fileit->second;
}

inline std::vector<std::string> FileCache::getFilenameList() const
{
    std::vector<std::string> files;
    files.reserve(cache.size());
    for(const auto& file : cache)
    {
        files.push_back(file.first);
    }
    return files;
}

inline std::vector<std::shared_ptr<File>> FileCache::getFiles() const
{
    std::vector<std::shared_ptr<File>> files;
    files.reserve(cache.size());
    for(const auto& f : cache)
    {
        files.push_back(f.second);
    }
    return files;
}

inline std::vector<std::shared_ptr<File>>
FileCache::getFilesByNames(const std::vector<std::string>& names) const
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

inline bool FileCache::addFile(std::shared_ptr<File> file, bool read)
{
    if(!file)
    {
        throw std::invalid_argument("Invalid file given to FileCache::addFile");
    }
    if(read)
    {
        if(!file->readFile())
        {
            return false;
        }
        if(!file->checkUtf8())
        {
            throw std::runtime_error("Invalid file encoding");
        }
    }
    cache.insert({file->filename, file});
    return true;
}
inline bool FileCache::addFile(const std::string& name, bool read)
{
    auto file = std::make_shared<File>(name);
    return addFile(std::move(file), read);
}
} // namespace util
