#file(GLOB_RECURSE sources_core *.cpp)

#add_library(core ${sources_core})
#llvm_map_components_to_libnames(llvm_libs support core irreader passes objcarcopts native)
#target_link_libraries(core util thirdparty ${llvm_libs})

add_subdirectory(lexer)
add_subdirectory(parser)

file(GLOB sources_core *.cpp)
file(GLOB headers_core *.h)

add_library(core ${sources_core} ${headers_core})
target_link_libraries(core core_parser core_lexer ast util thirdparty)

#add_library(core)
#target_link_libraries(core ast codegen lexer parser)
