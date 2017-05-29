#pragma once


#include "defines.h"

#ifdef win_x64_vc140
#include "Windows.h"
#elif osx_x64_clang
#include "unistd.h"
#include <sys/stat.h>
#include <dirent.h>
#else
#endif

// C++ STD
#include <set>
#include <string>
#include <type_traits>
#include <functional>
#include <vector>
#include <stack>
#include <map>

// INTERNAL
#include "signal.hpp"

//Vendor

#include "spdlog/fmt/fmt.h"
