if(BUILD_TESTS)
    file(GLOB_RECURSE sources_tests *.cpp)
    file(GLOB_RECURSE headers_tests *.h)

    add_executable(tests ${sources_tests} ${headers_tests})
    add_dependencies(tests varuna varuna-llvm-as varuna-opt varuna-llc)
    target_link_libraries(tests ast codegen core_lexer core_parser core util src)

    add_test(NAME varuna_tests COMMAND tests)
endif()
