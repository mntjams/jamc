#include "gmock/gmock.h"
#include "jamc/lex.hpp"

#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <ranges>
#include <string_view>
#include <vector>

namespace {
using ::jamc::lex::Lex;
using ::jamc::lex::Token;
using ::jamc::lex::TokenType;
using enum TokenType;

using ::testing::_;
using ::testing::AllOf;
using ::testing::ContainerEq;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::Matcher;

auto TokenMatches(Matcher<TokenType> const &type_matcher,
                  Matcher<std::size_t> const &line_matcher,
                  Matcher<std::size_t> const &col_matcher,
                  Matcher<std::string_view> const &lexeme_matcher)
    -> Matcher<Token const &> {
    return AllOf(Field("type", &Token::type, type_matcher),
                 Field("line", &Token::line, line_matcher),
                 Field("col", &Token::col, col_matcher),
                 Field("lexeme", &Token::lexeme, lexeme_matcher));
}

// TODO - One lexer test became a todo for parser tests:
// constant with a prefix of zeros 00000000123

// TODO - When the compiler gets more complicated create
// golden lexer tests. Have input sources in test/ and
// the expected golden files (stringified output of lexer).
// Implement a stringify method for the lexer output
// and compare stringified output with golden files.
// Allow rewriting of golden files with some flag -
// when a compiler changes I don't need to modify tens of
// test cases but just regenerate the golden files to match
// the current output.
// However this is dangerous on its own so keep these unit tests
// as well.

TEST(Lexer, EmptySource) {
    std::string_view constexpr source{};
    auto lex_res = Lex(source);
    EXPECT_THAT(lex_res, ElementsAre(TokenMatches(Eof, _, _, _)));
}

TEST(Lexer, CorrectTokensFromMinimalProgram) {
    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 2;\n"
        "}";
    auto lex_res = Lex(source);
    EXPECT_THAT(lex_res,
                ElementsAre(TokenMatches(Int, _, _, _),
                            TokenMatches(Ident, _, _, "main"),
                            TokenMatches(LParen, _, _, _),
                            TokenMatches(Void, _, _, _),
                            TokenMatches(RParen, _, _, _),
                            TokenMatches(LBrace, _, _, _),
                            TokenMatches(Return, _, _, _),
                            TokenMatches(Constant, _, _, "2"),
                            TokenMatches(Semic, _, _, _),
                            TokenMatches(RBrace, _, _, _),
                            TokenMatches(Eof, _, _, _)));
}

TEST(Lexer, CrazyButCorrectFormattingTokenPosition) {
    std::string_view constexpr source =
        "     int   main(  \n"
        "\n"
        " void\n"
        "   ) {return 2  \n"
        "\n"
        " ;}\n";
    auto lex_res = Lex(source);
    EXPECT_THAT(lex_res,
                ElementsAre(TokenMatches(Int, 1, 6, _),
                            TokenMatches(Ident, 1, 12, _),
                            TokenMatches(LParen, 1, 16, _),
                            TokenMatches(Void, 3, 2, _),
                            TokenMatches(RParen, 4, 4, _),
                            TokenMatches(LBrace, 4, 6, _),
                            TokenMatches(Return, 4, 7, _),
                            TokenMatches(Constant, 4, 14, _),
                            TokenMatches(Semic, 6, 2, _),
                            TokenMatches(RBrace, 6, 3, _),
                            TokenMatches(Eof, 7, 1, _)));
}

TEST(Lexer, SameInputWithDifferentWhitespacing) {
    namespace rg = std::ranges;
    namespace rv = rg::views;

    std::string_view constexpr source_minimal_whitespace =
        "int main(void){return 2;}";
    std::string_view constexpr source_crazy_format =
        "     int   main(  \n"
        "\n"
        " void\n"
        "   ) {return 2  \n"
        "\n"
        " ;}\n";

    auto lex_res_minimal = Lex(source_minimal_whitespace);
    auto lex_res_crazy = Lex(source_crazy_format);

    auto correct_token_types = std::vector<TokenType>{Int,
                                                      Ident,
                                                      LParen,
                                                      Void,
                                                      RParen,
                                                      LBrace,
                                                      Return,
                                                      Constant,
                                                      Semic,
                                                      RParen,
                                                      Eof};

    auto lex_res_minimal_token_types =
        lex_res_minimal |
        rv::transform([](auto &&token) { return token.type; }) |
        rg::to<std::vector>();
    auto lex_res_crazy_token_types =
        lex_res_crazy | rv::transform([](auto &&token) { return token.type; }) |
        rg::to<std::vector>();

    EXPECT_THAT(lex_res_minimal_token_types, ContainerEq(correct_token_types));
    EXPECT_THAT(lex_res_crazy_token_types, ContainerEq(correct_token_types));
}

TEST(Lexer, InvalidIntegerConstant) {
    std::string_view constexpr source = "123abc";
    auto lex_res = Lex(source);
    EXPECT_THAT(lex_res,
                ElementsAre(TokenMatches(Unknown, _, _, "123abc"),
                            TokenMatches(Eof, _, _, _)));
}

TEST(Lexer, ConstantSemicIdent) {
    std::string_view constexpr source = "123;abc";
    auto lex_res = Lex(source);
    EXPECT_THAT(lex_res,
                ElementsAre(TokenMatches(Constant, _, _, "123"),
                            TokenMatches(Ident, _, _, "abc"),
                            TokenMatches(Eof, _, _, _)));
}

TEST(Lexer, IdentifiersWithKeywordSuffixes) {
    std::array<std::string_view, 5> const idents{
        "abcvoid", "abcreturn", "abcint"};
    for (auto &&ident : idents) {
        auto lex_res = Lex(ident);
        EXPECT_THAT(lex_res,
                    ElementsAre(TokenMatches(Ident, _, _, ident),
                                TokenMatches(Eof, _, _, _)));
    }
}

TEST(Lexer, IdentifiersWithKeywordPrefixes) {
    std::array<std::string_view, 5> const idents{
        "voiding", "returning", "inting"};
    for (auto &&ident : idents) {
        auto lex_res = Lex(ident);
        EXPECT_THAT(lex_res,
                    ElementsAre(TokenMatches(Ident, _, _, ident),
                                TokenMatches(Eof, _, _, _)));
    }
}

TEST(Lexer, KeywordsAreCaseSensitive) {
    std::array<std::string_view, 5> const idents{"INT", "Return", "vOiD"};
    for (auto &&ident : idents) {
        auto lex_res = Lex(ident);
        EXPECT_THAT(lex_res,
                    ElementsAre(TokenMatches(Ident, _, _, ident),
                                TokenMatches(Eof, _, _, _)));
    }
}
}  // namespace
