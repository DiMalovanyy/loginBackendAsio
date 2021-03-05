cmake_minimum_required(VERSION 3.5)

project(Backend LANGUAGES C CXX VERSION 0.0.1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


add_subdirectory(lib)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/googletest)

add_subdirectory(logs)

add_subdirectory(src)
add_subdirectory(test)
