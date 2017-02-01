#file(GLOB_RECURSE sources_core *.cpp)

#add_library(core ${sources_core})
#llvm_map_components_to_libnames(llvm_libs support core irreader passes objcarcopts native)
#target_link_libraries(core util thirdparty ${llvm_libs})

add_subdirectory(ast)
add_subdirectory(codegen)
add_subdirectory(lexer)
add_subdirectory(parser)

#add_library(core)
#target_link_libraries(core ast codegen lexer parser)
