if(BUILD_TESTS)
    file(GLOB_RECURSE sources_tests *.cpp)

    add_executable(tests ${sources_tests})
    target_link_libraries(tests thirdparty ast codegen core_lexer core_parser core util)
endif()
