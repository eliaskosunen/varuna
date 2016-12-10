cmake_minimum_required(VERSION 3.2.3)
project(varuna)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
	"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CXX_STANDARD	"--std=c++14")

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(WARNINGS "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused")
	else()
		set(WARNINGS "-Weverything -Wno-c++98-compat -Wno-padded -Wno-unused -Wno-unused-parameter -Wno-weak-vtables -Wno-exit-time-destructors -Wno-global-constructors")
	endif()

	set(DEBUG_FLAGS		"-g -O0")
	set(RELEASE_FLAGS	"-O3")

	set(CMAKE_CXX_FLAGS_RELEASE	"${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS} ${CXX_STANDARD} -pthread")
	set(CMAKE_CXX_FLAGS_DEBUG	"${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS} ${WARNINGS} ${CXX_STANDARD} -pthread")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(CXX_STANDARD 	"/std:c++14")

	set(WARNINGS "/W4 /EHsc")

	set(DEBUG_FLAGS		"/DEBUG /Zi /Od")
	set(RELEASE_FLAGS	"/Ox")

	set(CMAKE_CXX_FLAGS_RELEASE	"${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS} ${WARNINGS} ${CXX_STANDARD}")
	set(CMAKE_CXX_FLAGS_DEBUG	"${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS} ${WARNINGS} ${CXX_STANDARD}")
endif()

set(CMAKE_CONFIGURATION_TYPES Debug Release)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	message(STATUS "No build configuration set, defaulting to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
else()
	message(STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	message(STATUS "Build flags: ${CMAKE_CXX_FLAGS_DEBUG}")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	message(STATUS "Build flags: ${CMAKE_CXX_FLAGS_RELEASE}")
else()
	message(STATUS "Unsupported build configuration: '${CMAKE_BUILD_TYPE}', defaulting to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
	message(STATUS "Build flags: ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(src)
include_directories(third-party/include)
include_directories(SYSTEM third-party/include-private)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(third-party)
add_subdirectory(src)
