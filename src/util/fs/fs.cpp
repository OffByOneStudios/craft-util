#include "util/common.h"
#include "fs.hpp"



using namespace craft;
using namespace craft::fs;

std::string fs::string_read(uint8_t d[], size_t s) {
	auto res = std::string((char*)d, s);
	delete[] d;

	return res;
};