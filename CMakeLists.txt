cmake_minimum_required(VERSION 2.8)
project(varuna)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
	"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(warnings "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wzero-as-null-pointer-constant -Wuseless-cast -Weffc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(warnings "/W4 /WX /EHsc")
endif()

set(CMAKE_C_FLAGS	"${CMAKE_C_FLAGS} ${warnings} --std=c11 -pthread")
set(CMAKE_CXX_FLAGS	"${CMAKE_CXX_FLAGS} ${warnings} --std=c++11 -pthread")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(src)
include_directories(SYSTEM third-party/include)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
