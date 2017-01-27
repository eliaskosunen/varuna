add_subdirectory(core)
add_subdirectory(fe)
add_subdirectory(tests)
add_subdirectory(util)

file(GLOB_RECURSE sources	Main.cpp)

add_executable(varuna ${sources})
target_link_libraries(varuna cli)
