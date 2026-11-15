#include "helpers.hpp"

#include "jamc/lex.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace jamc::test {
using ::testing::Matcher;
using ::testing::AllOf;
using ::testing::Field;

auto TokenMatches(Matcher<::jamc::lex::TokenType> const &type_matcher,
                  Matcher<std::size_t> const &line_matcher,
                  Matcher<std::size_t> const &col_matcher,
                  Matcher<std::string_view> const &lexeme_matcher)
    -> Matcher<::jamc::lex::Token const &> {
    return AllOf(Field("type", &::jamc::lex::Token::type, type_matcher),
                 Field("line", &::jamc::lex::Token::line, line_matcher),
                 Field("col", &::jamc::lex::Token::col, col_matcher),
                 Field("lexeme", &::jamc::lex::Token::lexeme, lexeme_matcher));
}
}  // namespace jamc::test
