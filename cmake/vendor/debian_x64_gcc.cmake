
SET(CRAFT_UTIL_LIBS "")
SET(CRAFT_UTIL_DEBUG_LIBS "")
SET(CRAFT_UTIL_RELEASE_LIBS "")

vendor_resolve_lib(curl CRAFT_UTIL_LIBS CRAFT_UTIL_DEBUG_LIBS CRAFT_UTIL_RELEASE_LIBS)

list(LENGTH CRAFT_UTIL_LIBS l)
if(NOT ${l} EQUAL 0)
target_link_libraries(CraftUtilTests ${CRAFT_UTIL_LIBS})
endif()

list(LENGTH CRAFT_UTIL_DEBUG_LIBS l)
if(NOT ${l} EQUAL 0)
target_link_libraries(CraftUtilTests debug ${CRAFT_UTIL_DEBUG_LIBS})
endif()

list(LENGTH CRAFT_UTIL_RELEASE_LIBS l)
if(NOT ${l} EQUAL 0)
target_link_libraries(CraftUtilTests optimized ${CRAFT_UTIL_RELEASE_LIBS})
endif()
