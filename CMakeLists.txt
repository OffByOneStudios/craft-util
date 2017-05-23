cmake_minimum_required(VERSION 3.7)


# External Cmake Deps
include(cmake/PrecompiledHeader.cmake)

# Preprocessor Defs
add_definitions(-DCRAFT_UTIL_DLL)

# File Operations
file(GLOB includes
  src/*.h
  src/*.hpp
  src/concurrency/*.h
  src/concurrency/*.hpp
)

file(GLOB sources
  src/*.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src)



add_library(craft_util SHARED
  ${sources}
  ${includes}
  common.cpp
)

add_precompiled_header(craft_util src/common.h FORCEINCLUDE)

# Platform Specific
if(MSVC)
  target_link_libraries(craft_util pathcch.lib shlwapi.lib)
endif()
