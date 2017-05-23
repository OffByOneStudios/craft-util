#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace craft {
namespace concurrency
{
	// Based on: http://stackoverflow.com/questions/15278343/c11-thread-safe-queue

	template <class T>
	class SafeQueue
	{
	private:
		// Sync
		mutable std::mutex m;
		std::condition_variable c;

		// Data
		std::queue<T> q;

	public:
		inline SafeQueue(void)
			: m(), c(), q()
		{}

		inline ~SafeQueue(void)
		{}

		inline void enqueue(T const& t)
		{
			std::lock_guard<std::mutex> lock(m);
			q.push(t);
			c.notify_one();
		}

		// Blocks if empty.
		inline T dequeue(void)
		{
			std::unique_lock<std::mutex> lock(m);

			while (q.empty()) c.wait(lock);

			T val = q.front(); q.pop();
			return val;
		}

		inline bool dequeueAntsy(T& val)
		{
			std::lock_guard<std::mutex> lock(m);

			if (q.empty()) return false;

			val = q.front(); q.pop();
			return true;
		}
	};
}}