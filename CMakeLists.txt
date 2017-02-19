cmake_minimum_required(VERSION 3.2.3)
project(varuna)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(BUILD_TESTS "Build unit tests" ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
	"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CXX_STANDARD	"--std=c++14")
	if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
		set(CXX_STANDARD	"--std=c++1y")
	endif()

	if (CMAKE_COMPILER_IS_GNUCXX)
		set(WARNINGS "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wno-switch-default -Wundef -Wno-unused -Wno-unused-parameter -Wno-date-time -Wno-overloaded-virtual")
	else()
		set(WARNINGS "-Weverything -Wno-c++98-compat -Wno-padded -Wno-unused -Wno-unused-parameter -Wno-weak-vtables -Wno-exit-time-destructors -Wno-global-constructors -Wno-date-time")
	endif()

	set(DEBUG_FLAGS				"-g -O0 -DVARUNA_DEBUG=1")
	set(RELEASE_FLAGS			"-O3 -DVARUNA_RELEASE=1")
	set(MINSIZEREL_FLAGS		"-Os -DVARUNA_RELEASE=1")
	set(RELWITHDEBINFO_FLAGS	"-g -O2 -DVARUNA_DEBUG=1")

	set(CMAKE_CXX_FLAGS_RELEASE			"${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS} ${CXX_STANDARD} -pthread")
	set(CMAKE_CXX_FLAGS_DEBUG			"${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS} ${WARNINGS} ${CXX_STANDARD} -pthread")
	set(CMAKE_CXX_FLAGS_MINSIZEREL		"${CMAKE_CXX_FLAGS_MINSIZEREL} ${MINSIZEREL_FLAGS} ${CXX_STANDARD} -pthread")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${RELWITHDEBINFO_FLAGS} ${WARNINGS} ${CXX_STANDARD} -pthread")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(WARNINGS "/W4 /EHsc /wd4100")

	set(DEBUG_FLAGS				"/DEBUG /Zi /Od /DVARUNA_DEBUG=1")
	set(RELEASE_FLAGS			"/Ox /DVARUNA_RELEASE=1")
	set(MINSIZEREL_FLAGS		"/O1 /DVARUNA_RELEASE=1")
	set(RELWITHDEBINFO_FLAGS	"/DEBUG /Zi /Ot /DVARUNA_DEBUG=1")

	set(CMAKE_CXX_FLAGS_RELEASE			"${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS} ${WARNINGS} /MTd")
	set(CMAKE_CXX_FLAGS_DEBUG			"${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS} ${WARNINGS} /MTd")
	set(CMAKE_CXX_FLAGS_MINSIZEREL		"${CMAKE_CXX_FLAGS_MINSIZEREL} ${MINSIZEREL_FLAGS} /MTd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${RELWITHDEBINFO_FLAGS} ${WARNINGS} /MTd")
endif()

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
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
	message(STATUS "Build flags: ${CMAKE_CXX_FLAGS_MINSIZEREL}")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
	message(STATUS "Build flags: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
else()
	message(STATUS "Unsupported build configuration: '${CMAKE_BUILD_TYPE}', defaulting to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
	message(STATUS "Build flags: ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM include directory: ${LLVM_INCLUDE_DIRS}")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(src)
include_directories(third-party/include)
include_directories(SYSTEM third-party/include-private)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(third-party)
add_subdirectory(src)

file(GLOB main_sources src/Main.cpp)

add_executable(varuna ${main_sources})
target_link_libraries(varuna src)
