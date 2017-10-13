cmake_minimum_required(VERSION 3.7)
project (CraftUtil)
if(TARGET CraftUtil)
    return()
endif(TARGET CraftUtil)

# This Variable is the Simplest way to ensure your variables will work
include(cmake/ResolveCraftRoot.cmake)
# This is mandatory
include(cmake/ResolveCraftCmake.cmake)

MESSAGE(STATUS "CRAFT_CMAKE_DIR " ${CRAFT_CMAKE_DIR})
# Use This To Support Multiplatforms or to use the Vendor Repo.
include(${CRAFT_CMAKE_DIR}/ResolveCraftTripleCross.cmake)


include(${CRAFT_CMAKE_DIR}/ResolveVendor.cmake)
# Use This is you depend on Vendors
IF(NOT DEFINED CRAFT_ENGINE_VENDOR_DIR)
  MESSAGE(FATAL_ERROR "Could not locate required dependency CRAFT_ENGINE_VENDOR_DIR")
ENDIF()

# External Cmake Deps
include(${CRAFT_CMAKE_DIR}/PrecompiledHeader.cmake)
include(${CRAFT_CMAKE_DIR}/MakeVSFilters.cmake)
include(${CRAFT_CMAKE_DIR}/MakeVSDocs.cmake)

# Use This Preprocess Def to Control exports
add_definitions(-D${CRAFT_TRIPLE_CROSS})


# File Operations
file(GLOB_RECURSE includes
  src/*.h
  src/*.hpp
)

file(GLOB_RECURSE sources
  src/*.cpp
)

file(GLOB_RECURSE tests
  test/*.cpp
)

set(files ${includes} ${sources})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CRAFT_VENDOR_INCLUDE_DIRECTORY}
)


add_library(CraftUtil SHARED
  ${sources}
  ${includes}
  common.cpp
)

target_compile_definitions(CraftUtil PRIVATE CRAFT_UTIL_DLL)


add_precompiled_header(CraftUtil src/util/common.h FORCEINCLUDE)

if(NOT DEFINED CRAFT_ENGINE_SUBDIRECTORY)
add_executable(CraftUtilTests
  ${tests}
)

target_link_libraries (CraftUtilTests CraftUtil)
endif()

#Vendor Dependencies
include(cmake/vendor/${CRAFT_TRIPLE_CROSS}.cmake)

# Platform Specific
if(CRAFT_TRIPLE_CROSS STREQUAL "osx_x64_clang")
  list(APPEND CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ -Wc++11-extensions")
  target_link_libraries(CraftUtil "-framework Security")
elseif(MSVC)
  set(files ${includes} ${sources} ${docs})
  target_link_libraries(CraftUtil pathcch.lib shlwapi.lib)
  VSFILTERS(${files})
  GenerateDocs(CraftUtil)
endif()
