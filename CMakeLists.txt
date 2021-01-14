cmake_minimum_required(VERSION 3.5)

project(Backend LANGUAGES C CXX VERSION 0.0.1)

add_subdirectory(lib/spdlog)
add_subdirectory(lib/googletest)

add_subdirectory(src)
add_subdirectory(test)
