# To-do list

- [ ] Parser
    - [ ] Print error if parsing failed
- [ ] Codegen
- [ ] Code emission

# To-do later
- [ ] Implement golden testing (when I can actually compile significant parts of the language)
    - Use env var "JAMC_UPDATE_GOLDENS=1"
        - When updating, print "goldens updated, tests fail deliberately" and fail the tests
    - Probably throught gtest, run my lex, read file, EXPECT_EQ
