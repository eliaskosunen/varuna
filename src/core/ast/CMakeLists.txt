file(GLOB_RECURSE sources_ast *.cpp)

add_library(ast ${sources_ast})
target_link_libraries(ast codegen)
