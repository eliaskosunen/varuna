file(GLOB sources_core_parser *.cpp)
file(GLOB headers_core_parser *.h)

add_library(core_parser ${sources_core_parser} ${headers_core_parser})
target_link_libraries(core_parser ast core_lexer util thirdparty)
