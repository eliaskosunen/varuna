file(GLOB_RECURSE sources_codegen *.cpp)

add_library(codegen ${sources_codegen})
llvm_map_components_to_libnames(llvm_libs support core irreader passes objcarcopts native)
target_link_libraries(codegen util thirdparty ast ${llvm_libs})
