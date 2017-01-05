// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "fe/api/FwdDecl.h"
#include "util/FileCache.h"
#include "util/Logger.h"
#include "util/ThreadPool.h"
#include <future>

namespace fe
{
namespace api
{
    class Application final
    {
    public:
        enum LoggingLevel
        {
            LOG_TRACE = spdlog::level::trace,
            LOG_DEBUG = spdlog::level::debug,
            LOG_INFO = spdlog::level::info,
            LOG_SILENT = spdlog::level::critical
        };

        explicit Application(size_t threads = 1)
            : pool(std::make_unique<util::ThreadPool>(threads)),
              fileCache(std::make_unique<util::FileCache>())
        {
        }
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;
        ~Application() = default;

        void setLoggingLevel(LoggingLevel l);

        bool addFile(const std::string& file);
        std::future<bool> runFile(const std::string& file);

        bool runFiles(std::vector<std::string> files);
        bool runAll();

        bool execute(std::vector<std::string> files);

    private:
        std::unique_ptr<util::ThreadPool> pool;
        std::unique_ptr<util::FileCache> fileCache;
    };
}
}
