file(GLOB_RECURSE sources_app *.cpp)

add_library(app ${sources_app})
target_link_libraries(app thirdparty util core)
