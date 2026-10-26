#include "gmock/gmock.h"
#include "jamc/lex.hpp"

#include <cstddef>
#include <gtest/gtest.h>
#include <string>
#include <string_view>

namespace {
using ::jamc::lex::ConstToken;
using ::jamc::lex::IdToken;
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
using ::testing::Pointwise;

auto TokenMatches(Matcher<TokenType> const &type_matcher,
                  Matcher<std::size_t> const &line_matcher,
                  Matcher<std::size_t> const &col_matcher)
    -> Matcher<Token const &> {
    return AllOf(Field("type", &Token::type, type_matcher),
                 Field("line", &Token::line, line_matcher),
                 Field("col", &Token::col, col_matcher));
}

auto IdTokenMatches(Matcher<TokenType> const &type_matcher,
                    Matcher<std::size_t> const &line_matcher,
                    Matcher<std::size_t> const &col_matcher,
                    Matcher<std::string_view> const &name_matcher)
    -> Matcher<IdToken const &> {
    // Match normal token + an ID name
    return AllOf(TokenMatches(type_matcher, line_matcher, col_matcher),
                 Field("name", &IdToken::name, name_matcher));
}

auto ConstTokenMatches(Matcher<TokenType> const &type_matcher,
                       Matcher<std::size_t> const &line_matcher,
                       Matcher<std::size_t> const &col_matcher,
                       Matcher<int> const &val_matcher)
    -> Matcher<ConstToken const &> {
    // Match normal token + an ID name
    return AllOf(TokenMatches(type_matcher, line_matcher, col_matcher),
                 Field("val", &ConstToken::val, val_matcher));
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
    EXPECT_THAT(lex_res.tokens,
                ElementsAre(TokenMatches(Int, _, _),
                            IdTokenMatches(Ident, _, _, "main"),
                            TokenMatches(LParen, _, _),
                            TokenMatches(Void, _, _),
                            TokenMatches(RParen, _, _),
                            TokenMatches(LBrace, _, _),
                            TokenMatches(Return, _, _),
                            ConstTokenMatches(Constant, _, _, 2),
                            TokenMatches(Semic, _, _),
                            TokenMatches(RBrace, _, _),
                            TokenMatches(Eof, _, _)));
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
    EXPECT_THAT(lex_res,
                ElementsAre(TokenMatches(Int, 1, 6),
                            IdTokenMatches(Ident, 1, 12, _),
                            TokenMatches(LParen, 1, 16),
                            TokenMatches(Void, 3, 2),
                            TokenMatches(RParen, 4, 4),
                            TokenMatches(LBrace, 4, 6),
                            TokenMatches(Return, 4, 7),
                            ConstTokenMatches(Constant, 4, 14, _),
                            TokenMatches(Semic, 6, 2),
                            TokenMatches(RBrace, 6, 3),
                            TokenMatches(Eof, 7, 1)));
}

TEST(Lexer, SameTokensForDifferentWhitespacing) {
    std::string const source_minimal_whitespace = "int main(void){return 2;}";
    std::string const source_crazy_format =
        "     int   main(  \n"
        "\n"
        " void\n"
        "   ) {return 2  \n"
        "\n"
        " ;}\n";
    auto lex_res_minimal_whitespace = Lex(source_minimal_whitespace);
    auto lex_res_crazy_format = Lex(source_crazy_format);

    EXPECT_THAT(Body(lex_res_minimal_whitespace).tokens,
                Pointwise(/* TODO - Matcher that compares token types */,
                          Body(lex_res_crazy_format).tokens));
}

// TODO - Implement the following
//      - Correct programs
//          - The output is same for program with many whitespaces and  program
//          with nearly no whitespaces
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
