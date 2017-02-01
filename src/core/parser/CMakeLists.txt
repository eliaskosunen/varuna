file(GLOB_RECURSE sources_parser *.cpp)

add_library(parser ${sources_parser})
target_link_libraries(parser ast)
