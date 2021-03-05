cmake_minimum_required(VERSION 3.5)

set(BINARY ${CMAKE_PROJECT_NAME}_logger)

add_library(${BINARY} STATIC logger.cpp)
set_target_properties(${BINARY} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
			LINKER_LANGUAGE CXX )

target_include_directories(${BINARY} PUBLIC ${CMAKE_BINARY_DIR}/include)
target_include_directories(${BINARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/spdlog)
target_link_libraries(${BINARY} spdlog)

