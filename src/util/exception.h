#pragma once
#include "common.h"

namespace stdext
{
	class exception
		: public std::exception
	{
	protected:
		std::string _msg;
		std::exception const& _inner;

		std::string _msg_fmt;

	public:
		/*inline explicit exception(const char* message)
			: _msg(message)
			, _inner(*this)
			, _msg_fmt()
		{ }

		inline explicit exception(const std::string& message)
			: _msg(message)
			, _inner(*this)
			, _msg_fmt()
		{ }

		inline explicit exception(const std::exception& inner, const std::string& message)
			: _msg(message)
			, _inner(inner)
		{
			_msg_fmt = fmt::format("{0}: {1}", _msg.c_str(), _inner.what());
		}*/

		template <typename... TArgs>
		inline explicit exception(const std::string& message, TArgs... args)
			: _inner(*this)
		{
			_msg = fmt::format(message, args...);
		}

		template <typename... TArgs>
		inline explicit exception(const std::exception& inner, const std::string& message, TArgs... args)
			: _inner(inner)
		{
			_msg = fmt::format(message, args...);
			_msg_fmt = fmt::format("{0}: {1}", _msg.c_str(), _inner.what());
		}

		inline bool hasInner() const noexcept { return !_msg_fmt.empty(); }
		inline std::exception const& inner() const noexcept { return _inner; }

		inline virtual ~exception() noexcept { }

		inline virtual const char* what() const noexcept {
			if (hasInner())
				return _msg_fmt.c_str();
			else
				return _msg.c_str();
		}
	};
}
