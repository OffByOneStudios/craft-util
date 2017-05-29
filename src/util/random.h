#pragma once
#include "common.h"
#include <random>

namespace craft {
	class WrappedRandom {

	private:
		long _seed;
		std::default_random_engine generator;
		std::uniform_real_distribution<double> distribution;
	public:

		CRAFT_UTIL_EXPORTED WrappedRandom(long seed);

		CRAFT_UTIL_EXPORTED double Sample();
		CRAFT_UTIL_EXPORTED void SetSeed(long seed);
		CRAFT_UTIL_EXPORTED void SetDistribution(double lbound, double ubound);
	};
}
