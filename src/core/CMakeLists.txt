add_subdirectory(lexer)
add_subdirectory(parser)

file(GLOB sources_core *.cpp)
file(GLOB headers_core *.h)

add_library(core ${sources_core} ${headers_core})
target_link_libraries(core core_parser core_lexer ast util)
