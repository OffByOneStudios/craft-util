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

		profiler(std::string const &n, std::shared_ptr<spdlog::logger> logger) 
			: _name(n)
			, _p(std::chrono::high_resolution_clock::now())
			, _logger(logger) { }

		~profiler()
		{
			using dura = std::chrono::duration<double>;
			auto d = std::chrono::high_resolution_clock::now() - _p;
			_logger->info("{0}: {1}", _name, std::chrono::duration_cast<dura>(d).count());
		}
	};
}