#pragma once
#include "common.h"

#include "exception.h"

namespace stdext
{
	//
	// Topological Sort
	//

	template<typename TRandomAccessIterator, typename TEdge>
	inline void topological_sort(TRandomAccessIterator first, TRandomAccessIterator last, TEdge edges)
	{
		std::map<TRandomAccessIterator, std::set<TRandomAccessIterator>> _dependencies;

		size_t count = 0;
		for (TRandomAccessIterator it = first; it != last; ++it)
		{
			std::set<TRandomAccessIterator> edge_set = edges(it);

			_dependencies[it] = edge_set;

			++count;
		}

		std::vector<TRandomAccessIterator> _order;

		size_t previous_size;
		size_t next_size = _dependencies.size();
		do {
			previous_size = next_size;
			for (TRandomAccessIterator it = first; it != last; ++it)
			{
				auto it_deps = _dependencies.find(it);
				if (it_deps == _dependencies.end()) continue;

				bool all_found = true;
				for (auto it_edge = it_deps->second.begin(); it_edge != it_deps->second.end(); ++it_edge)
				{
					if (_dependencies.find(*it_edge) != _dependencies.end())
					{
						all_found = false;
						break;
					}
				}
				if (!all_found) continue;

				_order.push_back(it);
				_dependencies.erase(it_deps);
			}
			next_size = _dependencies.size();
		} while (next_size != 0 && previous_size != next_size);

		if (next_size != 0)
			throw stdext::exception("Graph is cyclic.");

		std::map<TRandomAccessIterator, TRandomAccessIterator> _remappings;

		auto it_src = _order.begin();
		for (auto it_dest = first; it_src != _order.end(); ++it_src, ++it_dest)
		{
			auto source = *it_src;
			auto remap_it = _remappings.find(source);
			if (remap_it != _remappings.end())
				source = remap_it->second;

			std::iter_swap(source, it_dest);
			_remappings[it_dest] = source;
		}
	}

	//
	// Dictionary helpers
	//

	template <template<class, class, class...> class C, typename K, typename V, typename... Args>
	inline V get_with_default(const C<K, V, Args...>& m, K const& key, const V & defval)
	{
		typename C<K, V, Args...>::const_iterator it = m.find(key);
		if (it == m.end())
			return defval;
		return it->second;
	}

	//
	// String trimming
	//

	// trim from start
	inline ::std::string ltrim(::std::string const& s)
	{
		auto v = s;
		v.erase(v.begin(), std::find_if(v.begin(), v.end(),
			[](char c) { return !std::isspace(c); }));
		return v;
	}

	// trim from end
	inline ::std::string rtrim(::std::string const& s)
	{
		auto v = s;
		v.erase(v.begin() +
			(v.rend() - std::find_if(v.rbegin(), v.rend(),
				[](char c) { return !std::isspace(c); })));
		return v;
	}

	// trim from both ends
	inline ::std::string trim(::std::string const& s)
	{
		return ltrim(rtrim(s));
	}

	//
	// String Split / Join
	//

	template<class CharType, class OutputIterator>
	inline void split(std::basic_string<CharType> const& s, std::basic_string<CharType> const& split, OutputIterator out)
	{
		typedef typename std::basic_string<CharType>::size_type size_t;

		if (s == "") return;

		size_t window = split.size();
		size_t off = 0;
		while(true)
		{
			size_t loc = s.find(split, off);

			if (loc == std::string::npos) break;

			*out = s.substr(off, loc - off);
			++out;
			off = loc + window;
		}

		*out = s.substr(off);
		++out;
	}

	template<class CharType, class OutputIterator>
	inline void split(std::basic_string<CharType> const& s, CharType const* split, OutputIterator out)
	{
		::stdext::split(s, std::basic_string<CharType> { split }, out);
	}

	template<class CharType, class OutputIterator>
	inline void split(std::basic_string<CharType> const& s, CharType split, OutputIterator out)
	{
		::stdext::split(s, std::basic_string<CharType> { split }, out);
	}


	template<class CharType, class InputIterator>
	inline std::basic_string<CharType> join(std::basic_string<CharType> const& separator, InputIterator const& begin, InputIterator const& end, std::function <std::basic_string<CharType>(InputIterator const&)> transform)
	{
		std::basic_ostringstream<CharType> os;

		auto it = begin;
		bool cont = it != end;
		while (cont)
		{
			os << transform(it);

			++it;
			cont = it != end;

			if (cont) os << separator;
		}
		return os.str();
	}

	template<class CharType, class InputIterator,
		typename std::enable_if<std::is_literal_type<CharType>::value && !std::is_pointer<CharType>::value>::type* = nullptr>
		inline std::basic_string<CharType> join(std::basic_string<CharType> const& separator, InputIterator const& begin, InputIterator const& end)
	{
		return join<CharType, InputIterator>(std::basic_string<CharType>("") + separator, begin, end,
			[](InputIterator it) -> std::basic_string<CharType>  {return *it; });
	}

	template<class CharType, class InputIterator,
		typename std::enable_if<std::is_literal_type<CharType>::value && !std::is_pointer<CharType>::value>::type* = nullptr>
		inline std::basic_string<CharType> join(CharType separator, InputIterator const& begin, InputIterator const& end, std::function<CharType(InputIterator)> transform)
	{
		return join(std::basic_string<CharType>("") + separator, begin, end, transform);
	}

	template<class CharType, class InputIterator,
		typename std::enable_if<std::is_literal_type<CharType>::value && !std::is_pointer<CharType>::value>::type* = nullptr>
		inline std::basic_string<CharType> join(CharType separator, InputIterator const& begin, InputIterator const& end)
	{
		return join<CharType, InputIterator>(std::basic_string<CharType>("") + separator, begin, end,
			[](InputIterator it) -> std::basic_string<CharType> {return *it; });
	}


	template<class ContainerType>
	inline bool starts_with(ContainerType const& longer, ContainerType const& prefix)
	{
		if (prefix.end() - prefix.begin() > longer.end() - longer.begin())
			return false;
		return std::equal(prefix.begin(), prefix.end(), longer.begin());
	}

	template<class InputIterator, class CharType = typename std::iterator_traits<InputIterator>::value_type::value_type>
	inline std::basic_string<CharType> longest_common_prefix(InputIterator const& begin, InputIterator const& end, size_t offset = 0)
	{
		std::basic_ostringstream<CharType> os;
		auto it = begin;
		bool cont = it != end;
		size_t length = std::numeric_limits<size_t>::max();
		while (cont)
		{
			length = ((*it).size() < length) ? (*it).size() : length;
			++it;
			cont = it != end;
		}
		for (auto i = offset; i < length; i++)
		{
			it = begin;
			bool cont = it != end;
			CharType c = (*it)[i];
			while (cont)
			{
				if ((*it)[i] != c)
				{
					return os.str();
				}
				++it;
				cont = it != end;
			}
			os << c;
		}
		return os.str();
	}
}
