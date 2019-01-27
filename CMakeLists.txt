cmake_minimum_required(VERSION 3.13)
project(advdist)

set(CMAKE_CXX_STANDARD 17)

set(GLOBAL_ROOT .)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../../bin)

include_directories(lib/include)
add_subdirectory(lib)
add_subdirectory(apps)
