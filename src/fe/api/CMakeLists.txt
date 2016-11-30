file(GLOB_RECURSE sources_api *.cpp)

add_library(api ${sources_api})
target_link_libraries(api thirdparty util core)
