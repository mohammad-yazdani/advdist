cmake_minimum_required(VERSION 3.13)
project(client_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(client client.cc)
target_link_libraries(client PUBLIC advdist_lib)

add_executable(server server.cc)
target_link_libraries(server PUBLIC advdist_lib)
