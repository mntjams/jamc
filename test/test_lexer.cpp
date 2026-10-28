#include "gmock/gmock.h"
#include "jamc/lex.hpp"

#include <cstddef>
#include <gtest/gtest.h>
#include <ranges>
#include <string>
#include <string_view>

namespace {
using ::jamc::lex::Lex;
using ::jamc::lex::LexResult;
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

TEST(Lexer, CorrectTokensFromMinimalProgram) {
    std::string const source =
        "int main(void) {\n"
        "    return 2;\n"
        "}";
    auto lex_res = Lex(source);
    EXPECT_THAT(lex_res.tokens,
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
    std::string const source =
        "     int   main(  \n"
        "\n"
        " void\n"
        "   ) {return 2  \n"
        "\n"
        " ;}\n";
    auto lex_res = Lex(source);
    EXPECT_THAT(lex_res.tokens,
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

    std::string const source_minimal_whitespace = "int main(void){return 2;}";
    std::string const source_crazy_format =
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
        lex_res_minimal.tokens |
        rv::transform([](auto &&token) { return token.type; }) |
        rg::to<std::vector>();
    auto lex_res_crazy_token_types =
        lex_res_crazy.tokens |
        rv::transform([](auto &&token) { return token.type; }) |
        rg::to<std::vector>();

    EXPECT_THAT(lex_res_minimal_token_types, ContainerEq(correct_token_types));
    EXPECT_THAT(lex_res_crazy_token_types, ContainerEq(correct_token_types));
}

// TODO - Implement the following
//      - Incorrect programs - just lable all as "unexpected token"
//          - 'a-a' - invalid identifier
//          - '123abc` - invalid integer constant
//      - Edge cases
//          - 123;abc - shall produce constant 123, semic, identifier abc
//          - abcvoid, abcreturn, abcint, voiding, returning, inting - shall be
//          identifiers not keyword void, id ing (or keyword return, id ing)
//          - Empty file should be just eof
//          - keyword case sensitivity - Int, RETURN
//          - constants prefix - 000000123
}  // namespace
