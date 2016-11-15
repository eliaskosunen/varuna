cmake_minimum_required(VERSION 3.2.3)
project(varuna)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
	"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CXX_STANDARD	"--std=c++14")	# --std=c++11 or --std=c++14

	set(WARNINGS "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused")
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(WARNINGS "${WARNINGS} -Wlogical-op -Wnoexcept -Wstrict-null-sentinel -Wuseless-cast")
	endif()

	set(DEBUG_FLAGS		"-g -O1")
	set(RELEASE_FLAGS	"-O3")

	set(CMAKE_CXX_FLAGS			"${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS} ${WARNINGS} ${CXX_STANDARD} -pthread")
	set(CMAKE_CXX_FLAGS_DEBUG	"${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS} ${WARNINGS} ${CXX_STANDARD} -pthread")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(CXX_STANDARD 	"/std:c++14")	# Empty or /std:c++14

	set(WARNINGS "/W4 /EHsc")

	set(DEBUG_FLAGS		"/DEBUG /Zi /Od")
	set(RELEASE_FLAGS	"/Ox")

	set(CMAKE_CXX_FLAGS			"${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS} ${WARNINGS} ${CXX_STANDARD}")
	set(CMAKE_CXX_FLAGS_DEBUG	"${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS} ${WARNINGS} ${CXX_STANDARD}")
endif()

set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(Boost_USE_STATIC_LIBS		ON)
set(Boost_USE_MULTITHREADED		OFF)
set(Boost_USE_STATIC_RUNTIME	OFF)
find_package(Boost 1.62.0 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(src)
include_directories(third-party/include)
include_directories(SYSTEM third-party/include-private)

add_subdirectory(third-party)

add_subdirectory(src)

add_subdirectory(tests)
