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

    const auto& viewCache() const
    {
        return cache;
    }

    auto getCache()
    {
        return std::move(cache);
    }

    File* getFile(const std::string& name);
    std::vector<std::string> getFileList() const;

    bool addFile(std::unique_ptr<File> file, bool read = true);
    bool addFile(const std::string& name, bool read = true);

    std::unordered_map<std::string, std::unique_ptr<File>> cache;
};

inline File* FileCache::getFile(const std::string& name)
{
    auto fileit = cache.find(name);
    if(fileit == cache.end())
    {
        return nullptr;
    }
    return fileit->second.get();
}

inline std::vector<std::string> FileCache::getFileList() const
{
    std::vector<std::string> files;
    files.reserve(cache.size());
    for(const auto& file : cache)
    {
        files.push_back(file.first);
    }
    return files;
}

inline bool FileCache::addFile(std::unique_ptr<File> file, bool read)
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
    decltype(cache)::value_type elem{file->filename, std::move(file)};
    cache.insert(std::move(elem));
    return true;
}
inline bool FileCache::addFile(const std::string& name, bool read)
{
    auto file = std::make_unique<File>(name);
    return addFile(std::move(file), read);
}
} // namespace util
