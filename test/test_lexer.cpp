#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <string>

#include "jamc/core/lex.hpp"

namespace {
using ::jamc::core::lex::Lex;
using ::jamc::core::lex::Token;
using ::jamc::core::lex::TokenType;
using enum TokenType;

using ::testing::_;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::Matcher;

auto TokenMatches(Matcher<TokenType> type_matcher,
                  Matcher<std::size_t> line_matcher,
                  Matcher<std::size_t> col_matcher,
                  Matcher<std::string> name_matcher) -> Matcher<const Token&> {
    return AllOf(Field("type", &Token::type, type_matcher),
                 Field("line", &Token::line, line_matcher),
                 Field("col", &Token::col, col_matcher),
                 Field("name", &Token::name, name_matcher));
}

auto TokenIs(Matcher<TokenType> type_matcher, Matcher<std::string> name_matcher)
    -> Matcher<const Token&> {
    return TokenMatches(type_matcher, _, _, name_matcher);
}

auto TokenIs(Matcher<TokenType> type_matcher) -> Matcher<const Token&> {
    return TokenMatches(type_matcher, _, _, _);
}

auto TokenAt(Matcher<std::size_t> line_matcher,
             Matcher<std::size_t> col_matcher) -> Matcher<const Token&> {
    return TokenMatches(_, line_matcher, col_matcher, _);
}

TEST(Lexer, CorrectTokensFromMinimalProgram) {
    std::string source =
        "int main(void) {\n"
        "    return 2;\n"
        "}";
    auto lex_res = Lex(source);
    EXPECT_THAT(lex_res.tokens,
                ElementsAre(TokenIs(Int, "int"), TokenIs(Ident, "main"),
                            TokenIs(LParen, "("), TokenIs(Void, "void"),
                            TokenIs(RParen, ")"), TokenIs(LBrace, "{"),
                            TokenIs(Return, "return"), TokenIs(Constant, "2"),
                            TokenIs(Semic, ";"), TokenIs(RBrace, "}")));
}

// TODO - Come up with lexer tests
}  // namespace
