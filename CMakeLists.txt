cmake_minimum_required(VERSION 3.7)


# This Variable is the Simplest way to ensure your variables will work
include(cmake/ResolveCraftRoot.cmake)
# This is mandatory
include(cmake/ResolveCraftCmake.cmake)

MESSAGE(STATUS "CRAFT_CMAKE_DIR " ${CRAFT_CMAKE_DIR})
# Use This To Support Multiplatforms or to use the Vendor Repo.
include(${CRAFT_CMAKE_DIR}/ResolveCraftTripleCross.cmake)


include(${CRAFT_CMAKE_DIR}/ResolveVendor.cmake)
include(${CRAFT_CMAKE_DIR}/MakeVSFilters.cmake)
include (${CRAFT_CMAKE_DIR}/ImportDep.cmake)


# Use This is you depend on Vendors
IF(NOT DEFINED CRAFT_ENGINE_VENDOR_DIR)
  MESSAGE(FATAL_ERROR "Could not locate required dependency CRAFT_ENGINE_VENDOR_DIR")
ENDIF()


# External Cmake Deps
include(${CRAFT_CMAKE_DIR}/PrecompiledHeader.cmake)

# Preprocessor Defs
add_definitions(-DCRAFT_UTIL_DLL
    -D${CRAFT_TRIPLE_CROSS}
)

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

file(GLOB tests
  test/*.cpp
)

set(files ${includes} ${sources})

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CRAFT_VENDOR_INCLUDE_DIRECTORY}
)


add_library(CraftUtil SHARED
  ${sources}
  ${includes}
  common.cpp
)

add_precompiled_header(CraftUtil src/common.h FORCEINCLUDE)

add_executable(CraftUtilTests
  ${tests}
)

target_link_libraries (CraftUtilTests CraftUtil)

# Platform Specific
if(CRAFT_TRIPLE_CROSS STREQUAL "osx_x64_clang")
  list(APPEND CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ -Wc++11-extensions")
elseif(MSVC)
  set(files ${includes} ${sources} ${docs})
  target_link_libraries(CraftUtil pathcch.lib shlwapi.lib)
  VSFILTERS(${files})
endif()
