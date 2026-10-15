add_executable(test-lexer
    test_lexer.cpp
)
target_link_libraries(test-lexer
    PRIVATE jamc-core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test-lexer)
