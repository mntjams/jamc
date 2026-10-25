add_executable(test-cli-parser
    test_cli_parser.cpp
)
target_link_libraries(test-cli-parser
    PRIVATE jamc
    GTest::gmock_main
    common-compiler-flags
)

add_executable(test-lexer
    test_lexer.cpp
)
target_link_libraries(test-lexer
    PRIVATE jamc
    GTest::gmock_main
    common-compiler-flags
)

include(GoogleTest)
gtest_discover_tests(test-cli-parser)
gtest_discover_tests(test-lexer)
