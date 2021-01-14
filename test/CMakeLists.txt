cmake_minimum_required(VERSION 3.5)

project(Test VERSION 0.0.1)
set(BINARY ${CMAKE_PROJECT_NAME}_test)

set(SOURCES
	main.cpp
	)
set(HEADERS)

add_executable(${BINARY} ${SOURCES} ${HEADERS})
set_target_properties(${BINARY} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	)

target_include_directories(${BINARY} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_include_directories(${BINARY} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/googletest)

target_link_libraries(${BINARY} gtest)
