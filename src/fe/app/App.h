// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "fe/app/FwdDecl.h"
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
        explicit Application(size_t threads = 1)
            : pool(std::make_unique<util::ThreadPool>(threads)),
              fileCache(std::make_unique<util::FileCache>())
        {
        }
        Application(const Application&) = delete;
        Application(Application&&) noexcept = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) noexcept = delete;
        ~Application() noexcept = default;

        bool addFile(const std::string& filename);
        std::future<bool> runFile(std::shared_ptr<util::File> file);

        bool runFiles(std::vector<std::shared_ptr<util::File>> files);
        bool runAll();

        /// Execute the input file list from ProgramOptions
        bool execute();

    private:
        std::unique_ptr<util::ThreadPool> pool;
        std::unique_ptr<util::FileCache> fileCache;
    };
} // namespace api
} // namespace fe
