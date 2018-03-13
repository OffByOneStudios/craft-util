#pragma once


#include "util/defines.h"

constexpr int PLATFORM_CHECK(char const* lhs, char const* rhs)
{
	return (('\0' == lhs[0]) && ('\0' == rhs[0])) ? 0
		: (lhs[0] != rhs[0]) ? (lhs[0] - rhs[0])
		: PLATFORM_CHECK(lhs + 1, rhs + 1);
}

#ifdef WIN32
// Fix terrible windows header
#define NOMINMAX
// Windows include
//#include "Windows.h"
#else
#include "unistd.h"
#include <sys/stat.h>
#include <dirent.h>
#endif

// C++ STD
#include <fstream>
#include <limits>
#include <set>
#include <string>
#include <type_traits>
#include <algorithm>
#include <functional>
#include <regex>
#include <vector>
#include <queue>
#include <stack>
#include <future>
#include <map>
#include <sstream>

// INTERNAL
#include "util/signal.hpp"

//Vendor

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexpansion-to-defined"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#define CURL_STATICLIB
#include "curl/curl.h"
#include "h2o/picohttpparser.h"
