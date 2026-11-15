#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP

#include "jamc/lex.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <string_view>

namespace jamc::test {
using ::testing::Matcher;

auto TokenMatches(
    Matcher<::jamc::lex::TokenType> const &type_matcher,
    Matcher<std::size_t> const &line_matcher,
    Matcher<std::size_t> const &col_matcher,
    Matcher<std::string_view> const &lexeme_matcher)
    -> Matcher<::jamc::lex::Token const &>;
}  // namespace jamc::test

#endif
