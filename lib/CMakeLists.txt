cmake_minimum_required(VERSION 3.5)

project(Libs VERSION 0.0.1 LANGUAGES C CXX)
set(BINARY ${CMAKE_PROJECT_NAME}_lib)



#find_package(Boost REQUIRED)

#add_library(${BINARY} STATIC lib.h)
#set_target_properties(${BINARY} PROPERTIES
#	CXX_STANDARD 17
#	CXX_STANDARD_REQUIRED ON
#	)

#if(NOT Boost_FOUND)
#	message("Boost not found on pc. cloning libs from github")
#else()
#	message("Boost ${Boost_VERSION} was founded")
#	target_include_directories(${BINARY} PUBLIC ${Boost_INCLUDE_DIRS})

#	target_link_libraries(${BINARY} ${Boost_LIBRARIES})
#endif()



