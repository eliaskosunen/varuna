file(GLOB sources_codegen *.cpp)
file(GLOB headers_codegen *.h)

add_library(codegen ${sources_codegen})
llvm_map_components_to_libnames(llvm_libs_codegen support irreader passes objcarcopts native core codegen)
target_link_libraries(codegen ${llvm_libs_codegen} ast util thirdparty)
