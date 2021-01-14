cmake_minimum_required(VERSION 3.5)

project(SourceProj VERSION 0.0.1 LANGUAGES C CXX)

add_subdirectory(include)

set(BINARY ${CMAKE_PROJECT_NAME}_exe)

set(SOURCES
	main.cpp
	)
set(HEADERS
	)


add_executable(${BINARY} ${HEADERS} ${SOURCES})
set_target_properties(${BINARY} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	)
target_include_directories(${BINARY} PRIVATE ${CMAKE_BINARY_DIR}/include)
target_include_directories(${BINARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/spdlog)
target_link_libraries(${BINARY} spdlog)

find_package(Boost REQUIRED)
if (NOT Boost_FOUND)
	message(FATAL_ERROR "Could not found boost")
else()
	target_include_directories(${BINARY} PUBLIC ${Boost_INCLUDE_DIRS})
	target_link_libraries(${BINARY} ${Boost_LIBRARIES})
endif()

find_package(mongocxx REQUIRED)
target_link_libraries(${BINARY} mongo::mongocxx_shared)
