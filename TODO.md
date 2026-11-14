# To-do list

- [ ] Parser
    - [ ] Do try catch in tests and inspect the returned value (TODO in test_parser.cpp)
    - [ ] Check that tests fail (see below for problem on why)
    - [ ] Fix span past tokens problem (TODO in src/parse.cpp)
    - [ ] Fix non-DRY repetition of !Expect->throw Unexpected... (TODO in src/parse.cpp)
- [ ] Codegen
- [ ] Code emission

# To-do later
- [ ] Implement golden testing (when I can actually compile significant parts of the language)
    - Use env var "JAMC_UPDATE_GOLDENS=1"
        - When updating, print "goldens updated, tests fail deliberately" and fail the tests
    - Probably throught gtest, run my lex, read file, EXPECT_EQ
