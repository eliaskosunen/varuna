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

#include "util/ThreadPool.h"

namespace util
{
	thread_local bool ThreadPool::working = true;

	ThreadPool::ThreadPool(size_t threadCount)
		: stop(false), waiters(0)
	{
		if(!threadCount)
		{
			throw std::invalid_argument("ThreadPool needs more than 0 threads");
		}

		maxIdle = std::max<int>(1, static_cast<int>(threadCount));
		pendingStopSignal = 0;
		monitor = std::thread([&]()
		{
			recycle();
		});
	}

	ThreadPool::~ThreadPool() noexcept
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}

		condition.notify_all();
		for(auto &worker : workers)
		{
			worker.join();
		}

		monitor.join();
	}

	void ThreadPool::work()
	{
		working = true;

		while(working)
		{
			std::function<void()> task;

			{
				std::unique_lock<std::mutex> lock(queueMutex);

				++waiters;
				condition.wait(lock, [&]()
				{
					return stop || tasks.empty();
				});
				--waiters;

				if(stop && tasks.empty())
				{
					return;
				}
				task = std::move(tasks.front());
				tasks.pop();
			}

			task();
		}

		--pendingStopSignal;
	}

	void ThreadPool::recycle()
	{
		while(!stop)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);

			std::unique_lock<std::mutex> lock(queueMutex);
			if(stop)
			{
				return;
			}

			auto nw = waiters;
			nw -= pendingStopSignal;
			while(nw-- > maxIdle)
			{
				tasks.emplace([this]()
				{
					working = false;
				});
				condition.notify_one();
				++pendingStopSignal;
			}
		}
	}
}
