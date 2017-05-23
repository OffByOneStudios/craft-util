#pragma once
#include "common.h"

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
	// Iterator adapter for keys of maps
	//

	template <size_t N, typename TIterator>
	struct element_iterator : public TIterator
	{
		typedef typename std::tuple_element<N, typename std::iterator_traits<TIterator>::value_type>::type value_type;
		typedef value_type& reference;
		typedef value_type* pointer;
		typedef typename std::iterator_traits<TIterator>::difference_type difference_type;
		typedef typename std::iterator_traits<TIterator>::iterator_category iterator_category;

		typedef element_iterator iterator;

		inline element_iterator(TIterator const& it) : TIterator(it) { }

		inline pointer operator->() const { return &(std::get<N>(*TIterator::operator->())); }
		inline value_type operator*() const { return std::get<N>(TIterator::operator*()); }
	};

	template <typename TIterator>
	element_iterator<0, TIterator> key_iterator(TIterator const& it)
	{
		return { it };
	}

	template <typename TIterator>
	element_iterator<1, TIterator> value_iterator(TIterator const& it)
	{
		return { it };
	}

	template <size_t N, typename TIterator>
	element_iterator<N, TIterator> tuple_iterator(TIterator const& it)
	{
		return { it };
	}

	template <size_t N, typename TContainer>
	struct element_container_adaptor : public TContainer
	{
		typedef element_iterator<N, typename TContainer::const_iterator> const_iterator;

		inline element_container_adaptor(TContainer const& cn) : TContainer(cn) { }

		inline const_iterator begin() const { return const_iterator(TContainer::begin()); }
		inline const_iterator end() const { return const_iterator(TContainer::end()); }
		inline const_iterator cbegin() const { return const_iterator(TContainer::cbegin()); }
		inline const_iterator cend() const { return const_iterator(TContainer::cend()); }
	};

	template <typename TContainer>
	element_container_adaptor<0, TContainer> key_adaptor(TContainer const& cn)
	{
		return { cn };
	}

	template <typename TContainer>
	element_container_adaptor<1, TContainer> value_adaptor(TContainer const& cn)
	{
		return { cn };
	}

	template <size_t N, typename TContainer>
	element_container_adaptor<N, TContainer> tuple_adaptor(TContainer const& cn)
	{
		return { cn };
	}


	//
	// String trimming
	//

	// trim from start
	inline ::std::string ltrim(::std::string const& s)
	{
		auto v = s;
		v.erase(v.begin(), std::find_if(v.begin(), v.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
		return v;
	}

	// trim from end
	inline ::std::string rtrim(::std::string const& s)
	{
		auto v = s;
		v.erase(std::find_if(v.rbegin(), v.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), v.end());
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
	inline std::basic_string<CharType> join(CharType separator, InputIterator const& begin, InputIterator const& end)
	{
		std::basic_ostringstream<CharType> os;

		auto it = begin;
		bool cont = it != end;
		while (cont)
		{
			os << *it;

			++it;
			cont = it != end;

			if (cont) os << separator;
		}
		return os.str();
	}
	template<class InputIterator, class CharType = std::iterator_traits<InputIterator>::value_type::value_type>
	inline std::basic_string<CharType> longest_common_prefix(InputIterator const& begin, InputIterator const& end, size_t offset = 0)
	{
		std::basic_ostringstream<CharType> os;
		auto it = begin;
		bool cont = it != end;
		size_t length = MAXSIZE_T;
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
