#pragma once

#include <set>
#include <mutex>
#include <condition_variable>

namespace craft {
namespace concurrency
{
	template <class T>
	class SafeSet
	{
	private:
		// Sync
		mutable std::mutex m;

		// Data
		std::set<T> s;

	public:
		inline SafeSet(void)
			: s(), m()
		{ }

		inline ~SafeSet(void)
		{ }

		inline size_t size()
		{
			return s.size();
		}

		inline void insert(T& t)
		{
			std::lock_guard<std::mutex> lock(m);
			s.insert(t);
		}

		inline bool has(T& t)
		{
			std::lock_guard<std::mutex> lock(m);
			return s.count(t) != 0;
		}

		inline void erase(T& t)
		{
			std::lock_guard<std::mutex> lock(m);
			auto it = s.find(t);
			s.erase(it);
		}

		inline std::vector<T> asVector(void)
		{
			std::lock_guard<std::mutex> lock(m);
			return std::vector<T>(s.begin(), s.end());
		}
	};
}}
