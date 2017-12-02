
cmake_minimum_required(VERSION 3.7)
if(TARGET CraftEngineDepsUtil)
    return()
endif(TARGET CraftEngineDepsUtil)
Project(CraftEngineDepsUtil)

IF(NOT DEFINED CRAFT_ROOT_PROJECT)
	SET(CRAFT_ROOT_PROJECT CraftEngineDepsUtil)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
ENDIF()

# This Variable is the Simplest way to ensure your variables will work
include(cmake/ResolveCraftRoot.cmake)
# This is mandatory
include(cmake/ResolveCraftCmake.cmake)

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

# Dependent Projects


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CRAFT_VENDOR_INCLUDE_DIRECTORY}
  
)

# File Operations
set(LibSources
	src/util/algorithms.hpp
  src/util/all.h
  src/util/argparse.hpp
  src/util/basen.hpp
  src/util/common.h
  src/util/cpu.cpp
  src/util/cpu.h
  src/util/defines.h
  src/util/exception.h
  src/util/ider.hpp
  src/util/introspection.h
  src/util/iterators.hpp
  src/util/path.cpp
  src/util/path.h
  src/util/path_posix.cpp
  src/util/path_windows.cpp
  src/util/profiler.hpp
  src/util/random.h
  src/util/random.hpp
  src/util/shared_defines.h
  src/util/signal.hpp
  src/util/strings.hpp
  src/util/templates.h
  src/util/concurrency/SafeQueue.hpp
  src/util/concurrency/SafeSet.hpp
  src/util/concurrency/SharedBuffer.hpp
  src/util/fs/fs.cpp
  src/util/fs/fs.hpp
  src/util/net/fetch.cpp
  src/util/net/fetch.hpp
  src/util/net/http.cpp
  src/util/net/http.h
  src/util/net/HttpServer.cpp
  src/util/net/HttpServer.h
  src/util/net/TcpServer.cpp
  src/util/net/TcpServer.h
  src/util/threading/future.hpp
  src/util/threading/threading.h
  src/util/values/FloatValue.hpp
  src/util/values/IdValue.hpp
  src/util/values/IntValue.hpp
  
)

set(TestSources
	test/main.cpp
  test/reflect.py
  
)

set(EntrySources
	entry/main.cpp
  
)


link_directories(
  ${CRAFT_VENDOR_LIB_DIRECTORY}
)

add_library(CraftEngineDepsUtil SHARED
  ${LibSources}
  common.cpp
)

SET(CMAKE_CONFIGURATION_TYPES "Debug;Release;Trace")
SET(CMAKE_CXX_FLAGS_TRACE ${CMAKE_CXX_FLAGS_DEBUG})
SET(CMAKE_LINKER_FLAGS_TRACE ${CMAKE_LINKER_FLAGS_DEBUG})
SET(CMAKE_SHARED_LINKER_FLAGS_TRACE ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
SET(CMAKE_EXE_LINKER_FLAGS_TRACE ${CMAKE_EXE_LINKER_FLAGS_DEBUG})

set_property(TARGET CraftEngineDepsUtil
  APPEND PROPERTY COMPILE_DEFINITIONS
  $<$<CONFIG:Trace>:BUILD_TRACE>
  $<$<CONFIG:Debug>:BUILD_DEBUG>
  $<$<CONFIG:Release>:BUILD_RELEASE>
)


target_compile_definitions(CraftEngineDepsUtil PRIVATE CRAFT_UTIL_DLL)

add_precompiled_header(CraftEngineDepsUtil src/util/common.h FORCEINCLUDE)

target_link_libraries (CraftEngineDepsUtil )

# Tests
IF(${CRAFT_ROOT_PROJECT} STREQUAL CraftEngineDepsUtil)
	add_executable(CraftEngineDepsUtilTest ${TestSources})
	target_link_libraries(CraftEngineDepsUtilTest CraftEngineDepsUtil)
ENDIF()
# Entry
IF(${CRAFT_ROOT_PROJECT} STREQUAL CraftEngineDepsUtil)
	add_executable(CraftEngineDepsUtilEntry ${EntrySources})
	target_link_libraries(CraftEngineDepsUtilEntry CraftEngineDepsUtil)
ENDIF()

# IDE
set(files ${LibSources} ${TestSources} ${EntrySources})
VSFILTERS(${files})

#Vendor Dependencies
include(cmake/vendor/${CRAFT_TRIPLE_CROSS}.cmake)

# Platform Specific
include(cmake/platform/${CRAFT_TRIPLE_CROSS}.cmake)
include(cmake/flags/${CRAFT_TRIPLE_CROSS}.cmake)
include(cmake/defines/${CRAFT_TRIPLE_CROSS}.cmake)
