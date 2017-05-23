#pragma once
#include "common.h"

namespace cpu{
	class CPU {
		uint32_t _threads;
		std::string arch;
	public:
		CPU();
		CRAFT_UTIL_EXPORTED uint32_t threads();
	};

	CRAFT_UTIL_EXPORTED CPU* GetCPU();
}
