# TODO - Discover in a loop

add_library(test-helpers
    helpers.cpp
)
target_link_libraries(test-helpers
    PUBLIC jamc
    GTest::gmock
    common-compiler-flags
)
target_include_directories(test-helpers
    PUBLIC ${CMAKE_SOURCE_DIR}/test/include
)

add_executable(test-cli-parser
    test_cli_parser.cpp
)
target_link_libraries(test-cli-parser
    PRIVATE jamc
    test-helpers
    GTest::gmock_main
    common-compiler-flags
)

add_executable(test-lexer
    test_lexer.cpp
)
target_link_libraries(test-lexer
    PRIVATE jamc
    test-helpers
    GTest::gmock_main
    common-compiler-flags
)

add_executable(test-parser
    test_parser.cpp
)
target_link_libraries(test-parser
    PRIVATE jamc
    test-helpers
    GTest::gmock_main
    common-compiler-flags
)

include(GoogleTest)
gtest_discover_tests(test-cli-parser)
gtest_discover_tests(test-lexer)
gtest_discover_tests(test-parser)
