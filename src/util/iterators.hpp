#pragma once
#include "common.h"

namespace stdext
{
	/******************************************************************************
	** stdext::pointer_view
	******************************************************************************/

	template<typename T>
	class pointer_view
	{
	private:
		T* _data;
		std::size_t _size;

	public:
		typedef T* iterator_type;
		typedef T const* const_iterator_type;

		pointer_view(T* data, std::size_t size)
			: _data(data)
			, _size(size)
		{ }

		const_iterator_type begin() const { return _data; }
		const_iterator_type end() const { return _data + _size; }

		iterator_type begin() { return _data; }
		iterator_type end() { return _data + _size; }
	};

	/******************************************************************************
	** stdext::breadth_first_iterator
	******************************************************************************/

	template<typename T> struct breadth_first_iterator;

	template<typename T> inline bool operator == (breadth_first_iterator<T> const& this_, breadth_first_iterator<T> const& that_);
	template<typename T> inline bool operator != (breadth_first_iterator<T> const& this_, breadth_first_iterator<T> const& that_);

	template<typename T>
	struct breadth_first_iterator
		: std::iterator<std::input_iterator_tag, T>
	{
		using real_type = typename std::remove_pointer<T>::type;
		using sub_iterator_type = typename real_type::iterator_type;

	private:
		std::queue<T> _queue;

		inline void _push_queue_front()
		{
			for (auto value : *_queue.front())
				_queue.push(value);
		}

		friend bool operator == <>(breadth_first_iterator<T> const&, breadth_first_iterator<T> const&);
		friend bool operator != <>(breadth_first_iterator<T> const&, breadth_first_iterator<T> const&);

	public:
		breadth_first_iterator(T const& root)
		{
			_queue.push(root);
			_push_queue_front();
		}

		breadth_first_iterator()
		{
		}

		breadth_first_iterator& operator++()
		{
			do { _queue.pop(); } while (!_queue.empty() && _queue.front() == nullptr);

			if (!_queue.empty())
				_push_queue_front();

			return *this;
		}

		T const& operator*() const
		{
			return _queue.front();
		}
	};

	template<typename T>
	inline bool operator == (breadth_first_iterator<T> const& this_, breadth_first_iterator<T> const& that_)
	{
		return this_._queue.size() == that_._queue.size() && this_._queue == that_._queue;
	}
	template<typename T>
	inline bool operator != (breadth_first_iterator<T> const& this_, breadth_first_iterator<T> const& that_)
	{
		return this_._queue.size() != that_._queue.size() && this_._queue != that_._queue;
	}

	/******************************************************************************
	** stdext::depth_first_iterator
	******************************************************************************/

	template<typename T> struct depth_first_iterator;

	template<typename T> inline bool operator == (depth_first_iterator<T> const& this_, depth_first_iterator<T> const& that_);
	template<typename T> inline bool operator != (depth_first_iterator<T> const& this_, depth_first_iterator<T> const& that_);

	template<typename T>
	struct depth_first_iterator
		: std::iterator<std::input_iterator_tag, T>
	{
		using real_type = typename std::remove_pointer<T>::type;
		using sub_iterator_type = typename real_type::iterator_type;

	private:
		std::stack<T> _stack;

		inline void _push_stack_top()
		{
			auto top = _stack.top(); _stack.pop();
			std::vector<T> temp; temp.reserve(8);
			for (auto value : *top)
				temp.push_back(value);
			for (auto it = temp.rbegin(); it != temp.rend(); ++it)
				_stack.push(*it);
			_stack.push(top);
		}

		friend bool operator == <>(depth_first_iterator<T> const&, depth_first_iterator<T> const&);
		friend bool operator != <>(depth_first_iterator<T> const&, depth_first_iterator<T> const&);

	public:
		depth_first_iterator(T const& root)
		{
			_stack.push(root);
			_push_stack_top();
		}

		depth_first_iterator()
		{
		}

		depth_first_iterator& operator++()
		{
			do { _stack.pop(); } while (!_stack.empty() && _stack.top() == nullptr);

			if (!_stack.empty())
				_push_stack_top();

			return *this;
		}

		T const& operator*() const
		{
			return _stack.top();
		}
	};

	template<typename T>
	inline bool operator == (depth_first_iterator<T> const& this_, depth_first_iterator<T> const& that_)
	{
		return this_._stack.size() == that_._stack.size() && this_._stack == that_._stack;
	}
	template<typename T>
	inline bool operator != (depth_first_iterator<T> const& this_, depth_first_iterator<T> const& that_)
	{
		return this_._stack.size() != that_._stack.size() && this_._stack != that_._stack;
	}

	/******************************************************************************
	** stdext::breadth_first_view
	******************************************************************************/

	template<typename T>
	class breadth_first_view
	{
	private:
		T* _root;

	public:
		typedef breadth_first_iterator<T*> iterator_type;
		typedef breadth_first_iterator<T const*> const_iterator_type;

		breadth_first_view(T* const& root)
			: _root(root)
		{ }

		const_iterator_type begin() const { return const_iterator_type(_root); }
		const_iterator_type end() const { return const_iterator_type(); }

		iterator_type begin() { return iterator_type(_root); }
		iterator_type end() { return iterator_type(); }
	};

	template<typename T>
	inline breadth_first_view<T> do_breadth_first(T* const& root)
	{
		return breadth_first_view<T>(root);
	}

	/******************************************************************************
	** stdext::depth_first_view
	******************************************************************************/

	template<typename T>
	class depth_first_view
	{
	private:
		T* _root;

	public:
		typedef depth_first_iterator<T*> iterator_type;
		typedef depth_first_iterator<T const*> const_iterator_type;

		depth_first_view(T* const& root)
			: _root(root)
		{ }

		const_iterator_type begin() const { return const_iterator_type(_root); }
		const_iterator_type end() const { return const_iterator_type(); }

		iterator_type begin() { return iterator_type(_root); }
		iterator_type end() { return iterator_type(); }
	};

	template<typename T>
	inline depth_first_view<T> do_depth_first(T* const& root)
	{
		return depth_first_view<T>(root);
	}
}
