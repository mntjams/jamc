#include "jamc/lex.hpp"

#include <cstddef>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

namespace {
using ::jamc::core::lex::Lex;
using ::jamc::core::lex::LexResult;
using ::jamc::core::lex::Token;
using ::jamc::core::lex::TokenType;
using enum TokenType;

using ::testing::_;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::Matcher;

auto TokenMatches(Matcher<TokenType> const &type_matcher,
                  Matcher<std::size_t> const &line_matcher,
                  Matcher<std::size_t> const &col_matcher,
                  Matcher<std::string> const &name_matcher)
    -> Matcher<Token const &> {
    return AllOf(Field("type", &Token::type, type_matcher),
                 Field("line", &Token::line, line_matcher),
                 Field("col", &Token::col, col_matcher),
                 Field("name", &Token::name, name_matcher));
}

auto TokenIs(Matcher<TokenType> const &type_matcher,
             Matcher<std::string> const &name_matcher)
    -> Matcher<Token const &> {
    return TokenMatches(type_matcher, _, _, name_matcher);
}

auto TokenIs(Matcher<TokenType> const &type_matcher) -> Matcher<Token const &> {
    return TokenMatches(type_matcher, _, _, _);
}

auto TokenAt(Matcher<std::size_t> const &line_matcher,
             Matcher<std::size_t> const &col_matcher)
    -> Matcher<Token const &> {
    return TokenMatches(_, line_matcher, col_matcher, _);
}

auto Body(LexResult res) -> LexResult {
    if (res.tokens.back().type == Eof) {
        res.tokens.pop_back();
    }
    return res;
}

TEST(Lexer, CorrectTokensFromMinimalProgram) {
    std::string const source =
        "int main(void) {\n"
        "    return 2;\n"
        "}";
    auto lex_res = Lex(source);
    EXPECT_THAT(Body(lex_res).tokens,
                ElementsAre(TokenIs(Int, "int"),
                            TokenIs(Ident, "main"),
                            TokenIs(LParen, "("),
                            TokenIs(Void, "void"),
                            TokenIs(RParen, ")"),
                            TokenIs(LBrace, "{"),
                            TokenIs(Return, "return"),
                            TokenIs(Constant, "2"),
                            TokenIs(Semic, ";"),
                            TokenIs(RBrace, "}")));
}

// TODO - Come up with lexer tests
}  // namespace
