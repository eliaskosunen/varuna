file(GLOB_RECURSE sources_core *.cpp)

add_library(core ${sources_core})
llvm_map_components_to_libnames(llvm_libs support core irreader passes objcarcopts native)
target_link_libraries(core util thirdparty ${llvm_libs})
