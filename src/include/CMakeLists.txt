cmake_minimum_required(VERSION 3.5)

project(Include VERSION 0.0.1 LANGUAGES C CXX)

message(INFO "Configure files")

configure_file(
	version.h.in
	${CMAKE_BINARY_DIR}/include/version.h)

configure_file(
	constants.h.in
	${CMAKE_BINARY_DIR}/include/constants.h)

