string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-as-needed")

#---------------------------------------------------------------------
# pairhmm
#---------------------------------------------------------------------
set(TARGET power_pairhmm)
add_library(${TARGET} SHARED PowerPairHmm.c)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
