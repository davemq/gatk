project(PowerPairHMM)

find_package(JNI)
include_directories(${JNI_INCLUDE_DIRS})

string(APPEND CMAKE_CXX_FLAGS " -O3 -mvsx")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(UNIX)
  set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc -Wl,-z,now")
endif()

# add CMakeLists.txt in subdirectories
set(NATIVE_DIR ${PROJECT_SOURCE_DIR}/src/main/native)

add_subdirectory("${NATIVE_DIR}/pairhmm")
