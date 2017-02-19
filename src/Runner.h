// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "Dispatcher.h"
#include "util/FileCache.h"
#include "util/ThreadPool.h"

class Runner
{
public:
    explicit Runner(int threads = 1);

    bool run();

private:
    std::future<std::future<bool>> runFile(std::shared_ptr<util::File> f);
    std::future<bool> runCodegen(std::shared_ptr<ast::AST> a);
    std::future<bool> successTask();
    std::future<bool> failedTask();

    std::unique_ptr<util::ThreadPool> pool;
    std::unique_ptr<util::FileCache> fileCache;
};
