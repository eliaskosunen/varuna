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
#include <chrono>

namespace util
{
	// Based on: https://github.com/progschj/ThreadPool
	// Copyright (c) 2012 Jakob Progsch, Václav Zeman
	// Licensed under the zlib/libpng license
	class ThreadPool final
	{
		std::vector<std::thread> workers;
		std::queue<std::function<void()>> tasks;

		std::mutex queueMutex;
		std::condition_variable condition;
		bool stop;
		int waiters;

		static thread_local bool working;
		std::atomic_int maxIdle;
		std::atomic_int pendingStopSignal;
		std::thread monitor;

		void work();
		void recycle();
		
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

			if(waiters == 0)
			{
				std::thread t([&]()
				{
					work();
				});
				workers.push_back(std::move(t));
			}
		}

		condition.notify_one();
		return res;
	}
}
