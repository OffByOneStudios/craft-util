
SET(CRAFT_UTIL_LIBS "")
SET(CRAFT_UTIL_DEBUG_LIBS "")
SET(CRAFT_UTIL_RELEASE_LIBS "")

vendor_resolve_lib(libcurl CRAFT_UTIL_LIBS CRAFT_UTIL_DEBUG_LIBS CRAFT_UTIL_RELEASE_LIBS)
vendor_resolve_lib(picohttpparser CRAFT_UTIL_LIBS CRAFT_UTIL_DEBUG_LIBS CRAFT_UTIL_RELEASE_LIBS)

list(LENGTH CRAFT_UTIL_LIBS l)
if(NOT ${l} EQUAL 0)
target_link_libraries(CraftUtil ${CRAFT_UTIL_LIBS})
endif()

list(LENGTH CRAFT_UTIL_DEBUG_LIBS l)
if(NOT ${l} EQUAL 0)
target_link_libraries(CraftUtil debug ${CRAFT_UTIL_DEBUG_LIBS})
endif()

list(LENGTH CRAFT_UTIL_RELEASE_LIBS l)
if(NOT ${l} EQUAL 0)
target_link_libraries(CraftUtil optimized ${CRAFT_UTIL_RELEASE_LIBS})
endif()
