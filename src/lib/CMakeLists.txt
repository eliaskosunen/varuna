file(GLOB sources_cstd *.cpp)
file(GLOB headers_cstd *.h)

add_library(vacstd ${sources_cstd} ${headers_cstd})
add_dependencies(vacstd varuna vart)
add_custom_command(
    TARGET vacstd POST_BUILD
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/varuna ARGS ${CMAKE_SOURCE_DIR}/src/lib/cstd.va -emit=llvm-ir -logging=error
    COMMAND ${CMAKE_COMMAND} ARGS -E copy cstd.vamod ${CMAKE_SOURCE_DIR}
    COMMENT "Compiling standard library"
)
install(TARGETS vacstd DESTINATION lib)
