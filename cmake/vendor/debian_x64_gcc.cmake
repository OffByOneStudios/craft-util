

SET(CRAFT_UTIL_LIBS "")
SET(CRAFT_UTIL_DEBUG_LIBS "")
SET(CRAFT_UTIL_RELEASE_LIBS "")

vendor_resolve_lib(curl CRAFT_UTIL_LIBS CRAFT_UTIL_DEBUG_LIBS CRAFT_UTIL_RELEASE_LIBS)
vendor_resolve_lib(picohttpparser CRAFT_UTIL_LIBS CRAFT_UTIL_DEBUG_LIBS CRAFT_UTIL_RELEASE_LIBS)


foreach(lib ${CRAFT_UTIL_LIBS})
  target_link_libraries(CraftEngineDepsUtil ${lib})
endforeach()

foreach(lib ${CRAFT_UTIL_DEBUG_LIBS})
  target_link_libraries(CraftEngineDepsUtil debug ${lib})
endforeach()

foreach(lib ${CRAFT_UTIL_RELEASE_LIBS})
  target_link_libraries(CraftEngineDepsUtil optimized ${lib})
endforeach()

