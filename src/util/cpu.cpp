#include "util/common.h"
#include "cpu.h"

#ifdef WIN32
#include "Windows.h"
#endif

cpu::CPU* _cpu;

cpu::CPU::CPU() {
    _threads = 0;
#ifdef _WIN32
	SYSTEM_INFO s;
	GetSystemInfo(&s);
	_threads = s.dwNumberOfProcessors;
#endif

}

uint32_t cpu::CPU::threads() {
	return _threads;
}

cpu::CPU* cpu::GetCPU() {
	if (_cpu == nullptr) {
		_cpu = new cpu::CPU();
	}
	return _cpu;
}
