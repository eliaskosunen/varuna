/*
Copyright (C) 2016 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <future>

#include "fe/api/FwdDecl.h"
#include "util/Logger.h"
#include "util/FileCache.h"
#include "util/ThreadPool.h"

namespace fe
{
	namespace api
	{
		class Application final
		{
			std::unique_ptr<util::ThreadPool> pool;
			std::unique_ptr<util::FileCache> fileCache;

		public:
			enum LoggingLevel
			{
				LOG_TRACE = spdlog::level::trace,
				LOG_DEBUG = spdlog::level::debug,
				LOG_INFO = spdlog::level::info,
				LOG_SILENT = spdlog::level::critical
			};

			explicit Application(size_t threads = 1)
				: pool(std::make_unique<util::ThreadPool>(threads)), fileCache(std::make_unique<util::FileCache>()) {}
			Application(const Application&) = delete;
			Application(Application&&) = delete;
			Application &operator =(const Application&) = delete;
			Application &operator =(Application&&) = delete;
			~Application() = default;

			void setLoggingLevel(LoggingLevel l);

			bool addFile(const std::string &file);
			std::future<bool> runFile(const std::string &file);

			bool runFiles(std::vector<std::string> files);
			bool runAll();

			bool execute(std::vector<std::string> files);
		};
	}
}
