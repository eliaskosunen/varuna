// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

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
    class File
    {
    public:
        using Checksum_t = uint64_t;

        std::string filename, content;
        Checksum_t checksum;

        explicit File(std::string filename_)
            : filename(std::move(filename_)), checksum(0)
        {
        }

        bool readFile()
        {
            if(!content.empty())
            {
                return true;
            }
            try
            {
                content = StreamReader::readFile(filename);
            }
            catch(const std::exception& e)
            {
                util::logger->error(e.what());
                return false;
            }
            return true;
        }

        bool checkUtf8()
        {
            return utf8::is_valid(content.begin(), content.end());
        }

        [[noreturn]] void calculateChecksum()
        {
            throw std::logic_error(
                "util::FileCache::File::calculateChecksum() not implemented");
        }
    };

private:
    std::unordered_map<std::string, std::unique_ptr<File>> cache;

public:
    FileCache()
    {
    }

    File* getFile(const std::string& name)
    {
        auto fileit = cache.find(name);
        if(fileit == cache.end())
        {
            return nullptr;
        }
        return fileit->second.get();
    }

    const auto& viewCache() const
    {
        return cache;
    }

    auto getCache() -> decltype(cache)
    {
        return std::move(cache);
    }

    std::vector<std::string> getFileList() const
    {
        std::vector<std::string> files;
        files.reserve(cache.size());
        for(const auto& file : cache)
        {
            files.push_back(file.first);
        }
        return files;
    }

    bool addFile(std::unique_ptr<File> file, bool read = true)
    {
        if(!file)
        {
            throw std::invalid_argument(
                "Invalid file given to FileCache::addFile");
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
    bool addFile(const std::string& name, bool read = true)
    {
        auto file = std::make_unique<File>(name);
        return addFile(std::move(file), read);
    }
};
} // namespace util
