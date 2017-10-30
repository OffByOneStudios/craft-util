#pragma once


#include "util/defines.h"

#ifdef win_x64_vc140
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
#include <vector>
#include <queue>
#include <stack>
#include <future>
#include <map>

// INTERNAL
#include "util/signal.hpp"

//Vendor
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#define CURL_STATICLIB
#include "curl/curl.h"
#include "h2o/picohttpparser.h"