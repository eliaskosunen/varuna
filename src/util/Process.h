// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <string>

namespace util
{
class Process
{
    bool spawned{false};
    std::string file;
    std::string params;
    int returnValue{-1};

    bool _spawn();

public:
    Process(const std::string& pFile, const std::string& pParams)
        : file(pFile), params(pParams)
    {
    }
    Process(std::string&& pFile, std::string&& pParams)
        : file(std::move(pFile)), params(std::move(pParams))
    {
    }

    bool spawn();
    bool isSpawned() const;
    int getReturnValue() const;
    std::string getErrorString();

    static Process create(const std::string& pFile, const std::string& pParams)
    {
        Process p(pFile, pParams);
        return p;
    }
    static Process create(std::string&& pFile, std::string&& pParams)
    {
        Process p(std::move(pFile), std::move(pParams));
        return p;
    }
};
} // namespace util
