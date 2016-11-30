file(GLOB_RECURSE sources_util *.cpp)

add_library(util ${sources_util})
target_link_libraries(util thirdparty)
