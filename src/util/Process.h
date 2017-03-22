// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/Compatibility.h"
#include <string>

namespace util
{
class Process
{
    using ErrorCodeType = uint32_t;

    /// Has the process been spawned
    bool spawned{false};
    /// Executable
    std::string file;
    /// Command line arguments
    std::string params;
    /// Process exit status
    int returnValue{-1};
    /// Latest error code
    ErrorCodeType errorCode{0};

    /// Spawn the process.
    /// Platform-dependent
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

    /**
     * Spawn the process.
     * Throws std::logic_error if it has already been spawned
     * @return Success. For further error information, see getErrorString()
     */
    bool spawn();
    /// Has the process been spawned
    bool isSpawned() const;
    /**
     * Get process exit code.
     * Throws std::logic_error if the process hasn't been spawned
     * @return Exit code
     */
    int getReturnValue() const;
    /// Get platform-dependent error description.
    /// Returns an empty string if no error has occured
    std::string getErrorString();

    static std::string getSuccessErrorString();

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
