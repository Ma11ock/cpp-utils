# ryan-cpp-utils library build file.

# This file is part of ryan-cpp-utils.
set(UTILS_LIB ${CMAKE_PROJECT_NAME})
enable_language(C)
enable_language(CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0075 NEW)

set(SRC_SOURCES
  # Source files.
  util.cpp
  # Header files.
  util.hpp
  )

add_library(
    ${UTILS_LIB}
    ${SRC_SOURCES}
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
  target_compile_options(${UTILS_LIB} PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wconversion -Wsign-conversion>
  )
endif()


target_link_libraries(
  ${UTILS_LIB}
  PUBLIC
  ${CMAKE_REQUIRED_LIBRARIES}
  fmt
)


