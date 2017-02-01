file(GLOB sources_codegen *.cpp)
file(GLOB headers_codegen *.h)

add_library(codegen ${sources_codegen})
llvm_map_components_to_libnames(llvm_libs support core irreader passes objcarcopts native)
target_link_libraries(codegen ${llvm_libs})
