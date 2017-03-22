file(GLOB sources_core_lexer *.cpp)
file(GLOB headers_core_lexer *.h)

add_library(core_lexer ${sources_core_lexer} ${headers_core_lexer})
target_link_libraries(core_lexer util)
