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

set(files ${includes} ${sources})

include_directories(${CMAKE_SOURCE_DIR}/src)


add_library(craft_util SHARED
  ${sources}
  ${includes}
  common.cpp
)

add_precompiled_header(craft_util src/common.h FORCEINCLUDE)


# Platform Specific
if(MSVC)
  # VS Filters
  foreach(file IN LISTS files)
      get_filename_component(source_path "${file}" PATH)
      string(REPLACE ${CMAKE_SOURCE_DIR} "" sp_rel "${source_path}")
      string(REPLACE "/" "\\" sp "${sp_rel}")
      source_group( "${sp}" FILES "${file}")
  endforeach()
  source_group("PrecompiledHeader" FILES common.cpp)

  target_link_libraries(craft_util pathcch.lib shlwapi.lib)
endif()
