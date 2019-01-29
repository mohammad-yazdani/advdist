cmake_minimum_required(VERSION 3.13)
project(advdist_lib)

set(CMAKE_CXX_STANDARD 17)

add_library(advdist_lib include/time/Event.h include/net/Socket.h src/time/Event.cc src/net/Socket.cc src/time/Benchmark.cc include/time/Benchmark.h)

