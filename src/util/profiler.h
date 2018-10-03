#pragma once

#include "common.h"

namespace stdext
{
	class profiler
	{
		std::string _name;
		std::chrono::high_resolution_clock::time_point _p;
		std::shared_ptr<spdlog::logger> _logger;

	public:
		profiler(std::string const &n, std::shared_ptr<spdlog::logger> logger) ;
		~profiler();
	};
}
