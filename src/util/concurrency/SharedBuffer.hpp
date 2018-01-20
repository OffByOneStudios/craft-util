#pragma once

#include <list>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace craft {
namespace concurrency
{
	// A buffer for one writer and many readers.
	template <class T>
	class SharedBuffer
	{
		typedef int MarkerId;

	public:
		class Marker
		{
			friend class SharedBuffer<T>;

			MarkerId _markerId;
			SharedBuffer<T>* _eventList;

			Marker(SharedBuffer<T>* _list, MarkerId _id)
				: _markerId(_id), _eventList(_list)
			{ }
		public:
			Marker()
				: _markerId(-1), _eventList(nullptr)
			{ }

			inline bool isValid() const
			{
				return _markerId >= 0 && _eventList != nullptr;
			}

			inline T const& top()
			{
				return _eventList->_data[_eventList->_markers[_markerId] % _eventList->_bufferSize];
			}

			inline T const& pop()
			{
				assert(!empty());
				return _eventList->_data[_eventList->_markers[_markerId]++ % _eventList->_bufferSize];
			}

			inline bool empty()
			{
				return _eventList->_write - _eventList->_markers[_markerId] == 0;
			}
			
			// TODO: For interface
		};
		friend class Marker;
	private:

		// Capacity
		size_t _bufferSize;
		size_t _maxMarkers;

		// Sync
		mutable std::mutex m_attachMarker;
		std::atomic<MarkerId> _nextMarker;

		std::atomic<uint64_t> _write;
		uint64_t _trailingRead;

		// Data
		std::atomic<uint64_t>* _markers;
		T* _data;
	public:
		inline SharedBuffer(size_t buffer_size = 1024, size_t max_markers = 128)
			: _bufferSize(buffer_size), _maxMarkers(max_markers)
			, m_attachMarker(), _nextMarker(0)
			, _write(0), _trailingRead(0)
		{
			_data = new T[buffer_size];
			_markers = new std::atomic<uint64_t>[max_markers];
		}

		inline ~SharedBuffer(void)
		{
			delete[] _data;
			delete[] _markers;
		}

		inline Marker marker(MarkerId id = -1)
		{
			if (id != -1)
				return Marker(this, id);

			std::lock_guard<std::mutex> lock(m_attachMarker);

			if (_nextMarker >= _maxMarkers)
				throw stdext::exception("No markers left.");

			_markers[_nextMarker] = (uint64_t)_write;

			return Marker(this, _nextMarker++);
		}

		inline uint64_t trailing() const
		{
			if (_nextMarker == 0) return _write;
			uint64_t ret = UINT64_MAX;
			MarkerId count = _nextMarker;
			for (int i = 0; i < count; i++)
			{
				ret = std::min<uint64_t>(ret, _markers[i]);
			}
			return ret;
		}

		inline void recalc()
		{
			_trailingRead = trailing();
		}

		inline bool full() const
		{
			return _write - _trailingRead == _bufferSize;
		}

		inline void push(T const& value)
		{
			assert(!full());
			_data[_write++ % _bufferSize] = value;
		}

		inline uint64_t countWritten() const
		{
			return _write;
		}

		inline uint64_t countPending() const
		{
			return _write - _trailingRead;
		}

		inline uint64_t capacity() const
		{
			return _bufferSize;
		}

		// TODO: Implement helpers for various full buffer stratigies:
		// * Shift Blocking Readers By 1
		// * Reset all Blocking Readers (returning a list of them)
	};
}}