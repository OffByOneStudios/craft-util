
#Override
macro(recursive_lookup dir)
IF(EXISTS ${dir}/CraftEngine.lock)
SET(CRAFT_ENGINE_ROOT_DIR ${dir})
# SET(${dir} PARENT_SCOPE)
return()
ENDIF()
IF(${dir} STREQUAL "/")
return()
ENDIF()
get_filename_component(TMP ${dir} DIRECTORY)

recursive_lookup(${TMP})
endmacro(recursive_lookup)

IF(DEFINED CRAFT_ENGINE_ROOT_DIR)
return()
ELSEIF(DEFINED $ENV{CRAFT_ENGINE_ROOT_DIR})
SET(CRAFT_ENGINE_ROOT_DIR $ENV{CRAFT_ENGINE_ROOT_DIR})
return()
ELSE()
recursive_lookup(${CMAKE_CURRENT_SOURCE_DIR})
ENDIF()

IF(NOT DEFINED CRAFT_ENGINE_ROOT_DIR)
message(STATUS "Repository was not checked out as a submodule, And CRAFT_ENGINE_ROOT_DIR was not set")
ENDIF()

