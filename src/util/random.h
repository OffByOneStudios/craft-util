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

		WrappedRandom(long seed);

		double Sample();
		void SetSeed(long seed);
		void SetDistribution(double lbound, double ubound);
	};
}
