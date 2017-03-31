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
    /// \throw std::runtime_error (Windows only) Return value getting failed
    bool _spawn();

public:
    Process(std::string pFile, std::string pParams);

    /**
     * Spawn the process.
     * \throw  std::logic_error   If process had already been spawned
     * \throw  std::runtime_error (Windows only) Return value getting failed
     * \return Success. For further error information, see getErrorString()
     */
    bool spawn();
    /// Has the process been spawned
    bool isSpawned() const;
    /**
     * Get process exit code.
     * \throw std::logic_error If the process hasn't been spawned
     * \return Exit code
     */
    int getReturnValue() const;
    /// Get platform-dependent error description.
    /// Returns an empty string if no error has occured
    std::string getErrorString();

    static std::string getSuccessErrorString();
};
} // namespace util
