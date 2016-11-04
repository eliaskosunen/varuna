file(GLOB_RECURSE sources	app/*.cpp core/*.cpp util/*.cpp)

add_library(src ${sources})
add_executable(varuna Main.cpp)
target_link_libraries(varuna src)
