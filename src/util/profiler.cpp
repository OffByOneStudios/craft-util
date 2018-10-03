#pragma once

#include "profiler.h"

using namespace stdext;

profiler::profiler(std::string const &n, std::shared_ptr<spdlog::logger> logger) 
	: _name(n)
	, _p(std::chrono::high_resolution_clock::now())
	, _logger(logger)
{ }

profiler::~profiler()
{
	long long d = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _p).count();
	_logger->info("{0}: {1}", _name, d);
}
