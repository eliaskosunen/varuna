file(GLOB_RECURSE sources_cli *.cpp)

add_library(cli ${sources_cli})
target_link_libraries(cli app thirdparty util core)
