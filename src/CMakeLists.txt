add_subdirectory(ast)
add_subdirectory(codegen)
add_subdirectory(core)
add_subdirectory(util)
add_subdirectory(tests)

file(GLOB src_sources CLI.cpp Runner.cpp)
file(GLOB src_headers CLI.h Dispatcher.h Doc.h Runner.h)

add_library(src ${src_sources} ${src_headers})
llvm_map_components_to_libnames(llvm_libs_src option)
target_link_libraries(src ${llvm_libs_src} util ast core codegen util thirdparty)
