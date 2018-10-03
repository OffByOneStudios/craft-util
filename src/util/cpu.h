#pragma once
#include "common.h"

namespace cpu{
	class CPU {
		uint32_t _threads;
		std::string arch;
	public:
		CPU();
		uint32_t threads();
	};

	CPU* GetCPU();
}
