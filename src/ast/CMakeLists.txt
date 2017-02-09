file(GLOB sources_ast *.cpp)
file(GLOB headers_ast *.h)

add_library(ast ${sources_ast} ${headers_ast})
target_link_libraries(ast codegen util thirdparty)
