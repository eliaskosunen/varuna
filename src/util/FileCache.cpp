// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/FileCache.h"

namespace util
{
std::shared_ptr<File> FileCache::getFile(const std::string& name, bool add)
{
    // Only add if requested
    if(add)
    {
        if(!addFile(name))
        {
            return nullptr;
        }
    }

    // Search the cache
    auto fileit = cache.find(name);
    if(fileit == cache.end())
    {
        return nullptr;
    }
    return fileit->second;
}

std::vector<std::string> FileCache::getFilenameList() const
{
    std::vector<std::string> files;
    files.reserve(cache.size());

    for(const auto& file : cache)
    {
        files.push_back(file.first);
    }
    return files;
}

std::vector<std::shared_ptr<File>> FileCache::getFiles() const
{
    std::vector<std::shared_ptr<File>> files;
    files.reserve(cache.size());

    for(const auto& f : cache)
    {
        files.push_back(f.second);
    }
    return files;
}

bool FileCache::addFile(std::shared_ptr<File> file, bool read)
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
    }
    cache.insert({file->getFilename(), file});
    return true;
}
bool FileCache::addFile(const std::string& name, bool read)
{
    return addFile(std::make_shared<File>(name), read);
}
} // namespace util
