#include "common.h"
#include "random.h"

using namespace craft;

WrappedRandom::WrappedRandom(long seed)
	: _seed(seed)
	, generator(seed)
	, distribution(-1.0, 1.0)
{

}

double WrappedRandom::Sample() {
	return distribution(generator);
}

void WrappedRandom::SetSeed(long seed) {
	generator = std::default_random_engine(seed);
}

void WrappedRandom::SetDistribution(double lbound, double ubound) {
	distribution = std::uniform_real_distribution<double>(lbound, ubound);
}
