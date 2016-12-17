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

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace util
{
	// Based on: https://github.com/progschj/ThreadPool
	// Copyright (c) 2012 Jakob Progsch, Václav Zeman
	// Licensed under the zlib/libpng license
	class ThreadPool final
	{
		size_t threads;
		std::vector<std::thread> workers;
		std::queue<std::function<void()>> tasks;
		std::mutex queueMutex;
		std::condition_variable condition;
		bool stop {false};
	public:
		ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
		~ThreadPool() noexcept;

		template <typename F, typename... Args>
		auto enqueue(F&& f, Args&&... args)
			-> std::future<typename std::result_of<F(Args...)>::type>;

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool &operator =(const ThreadPool&) = delete;
		ThreadPool &operator =(ThreadPool&&) = delete;
	};

	inline ThreadPool::ThreadPool(size_t threadCount)
	{
		if(!threadCount)
		{
			throw std::invalid_argument("ThreadPool needs more than 0 threads");
		}

		workers.reserve(threadCount);
		for(; threadCount; --threadCount)
		{
			workers.emplace_back([&]()
			{
				while(true)
				{
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lock(queueMutex);

						condition.wait(lock, [&]()
						{
							return stop || !tasks.empty();
						});
						if(stop && tasks.empty())
						{
							return;
						}

						task = std::move(tasks.front());
						tasks.pop();
					}

					task();
				}
			});
		}
	}

	inline ThreadPool::~ThreadPool() noexcept
	{
		stop = true;
		condition.notify_all();
		for(auto &worker : workers)
		{
			worker.join();
		}
	}

	template <typename F, typename... Args>
	inline auto ThreadPool::enqueue(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_t = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared<std::packaged_task<return_t()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<return_t> res = task->get_future();

		{
			std::unique_lock<std::mutex> lock(queueMutex);

			if(stop)
			{
				throw std::logic_error("Cannot enqueue on stopped ThreadPool");
			}

			tasks.emplace([&]()
			{
				(*task)();
			});
		}

		condition.notify_one();
		return res;
	}
}
