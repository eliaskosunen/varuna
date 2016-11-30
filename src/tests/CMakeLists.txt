file(GLOB_RECURSE sources_tests *.cpp)

add_executable(tests ${sources_tests})
target_link_libraries(tests thirdparty core api cli util)
