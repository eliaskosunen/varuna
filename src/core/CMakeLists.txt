file(GLOB_RECURSE sources_core *.cpp)

add_library(core ${sources_core})
target_link_libraries(core util thirdparty)
