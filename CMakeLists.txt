# This file is part of ryan-cpp-utils.
cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE release)
  message("ryancpputils: Setting build type to release.")
else()
  set(CMAKE_BUILD_TYPE debug)
  set(LAUNCHER_TESTS 1)
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
  message("ryancpputils: Build type is set to debug.")
endif()

project(ryancpputils)

add_subdirectory(external/fmt)

# Source.
add_subdirectory(src)


