cmake_minimum_required(VERSION 3.30)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

project("jamc"
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Shall be inherited by all downstream users
# (INTERFACE = the target doesn't use it but all downstream should)
add_library(common-compiler-flags INTERFACE)
target_compile_options(common-compiler-flags INTERFACE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Wpedantic>
)


include(CTest)
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.18.0
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(src/jamc/)
if (BUILD_TESTING)
    add_subdirectory(test)
endif()
