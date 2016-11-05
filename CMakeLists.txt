cmake_minimum_required(VERSION 2.8.12)
project(varuna)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
	"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(WARNINGS "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wuseless-cast")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(WARNINGS "/W4 /WX /EHsc")
endif()

set(CXX_STANDARD 	"11")
set(DEBUG_FLAGS		"-g -O1")
set(RELEASE_FLAGS	"-O3")

set(CMAKE_CXX_FLAGS			"${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS} ${WARNINGS} --std=c++${CXX_STANDARD} -pthread")
set(CMAKE_CXX_FLAGS_DEBUG	"${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS} ${WARNINGS} --std=c++${CXX_STANDARD} -pthread")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(src)
include_directories(SYSTEM third-party/include)

add_subdirectory(third-party)

add_subdirectory(src)

add_subdirectory(tests)
