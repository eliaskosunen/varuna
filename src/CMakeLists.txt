add_subdirectory(ast)
add_subdirectory(codegen)
add_subdirectory(core)
add_subdirectory(util)

file(GLOB src_sources CLI.cpp Runner.cpp)
file(GLOB src_headers CLI.h Dispatcher.h Doc.h Runner.h)

add_library(src ${src_sources} ${src_headers})
llvm_map_components_to_libnames(llvm_libs_src option)
target_link_libraries(src ${llvm_libs_src} util ast core codegen util thirdparty)

if(COVERALLS)
    file(GLOB_RECURSE coveralls_sources ast/*.cpp codegen/*.cpp core/*.cpp util/*.cpp CLI.cpp Runner.cpp)
    coveralls_setup(
        "${coveralls_sources}"
        ON
    "${PROJECT_SOURCE_DIR}/scripts/coveralls-cmake/cmake")
endif()
