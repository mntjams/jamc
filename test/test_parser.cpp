#include "gmock/gmock.h"
#include "helpers.hpp"
#include "jamc/errors.hpp"
#include "jamc/lex.hpp"
#include "jamc/parse.hpp"

#include <gtest/gtest.h>

namespace {
using ::jamc::lex::Lex;
using ::jamc::parse::Constant;
using ::jamc::parse::Function;
using ::jamc::parse::ParseProgram;
using ::jamc::parse::Program;
using ::jamc::parse::ReturnExpression;
using ::jamc::parse::Statement;
using ::jamc::test::TokenMatches;

using ::testing::_;
using ::testing::ElementsAre;

TEST(Parser, CorrectMinimalProgram) {
    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 2;\n"
        "}";
    auto lex_res = Lex(source);
    auto parse_res = ParseProgram(lex_res);
    auto expect =
        Program{.function_definition = Function{
                    .name = "main",
                    .body = Statement{.exp = ReturnExpression{
                                          .constant = Constant{.val = 2}}}}};
    EXPECT_EQ(expect, parse_res);
}

TEST(Parser, IntWithLeadingZeros) {
    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 00000000000000000000000000000123;\n"
        "}";
    auto lex_res = Lex(source);
    auto parse_res = ParseProgram(lex_res);
    auto expect =
        Program{.function_definition = Function{
                    .name = "main",
                    .body = Statement{.exp = ReturnExpression{
                                          .constant = Constant{.val = 123}}}}};
    EXPECT_EQ(expect, parse_res);
}

TEST(Parser, ValueOutOfIntRange) {
    using ::jamc::InvalidConstantError;
    using ::jamc::lex::Token;
    using enum ::jamc::lex::TokenType;

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 123456789123456789;\n"
        "}";
    bool threw_expected_type{false};

    auto lex_res = Lex(source);
    try {
        ParseProgram(lex_res);
    } catch (InvalidConstantError const &err) {
        threw_expected_type = true;

        std::string_view constexpr const_lexeme{"123456789123456789"};
        EXPECT_EQ(err.lexeme, const_lexeme);
    }
    EXPECT_TRUE(threw_expected_type);
}

TEST(Parser, BadFunctionDefinition) {
    using ::jamc::UnexpectedTokenError;
    using enum ::jamc::lex::TokenType;

    // Missing "void)"
    std::string_view constexpr source =
        "int main( {\n"
        "    return 2;\n"
        "}";
    bool threw_expected_type{false};

    auto lex_res = Lex(source);
    try {
        ParseProgram(lex_res);
    } catch (UnexpectedTokenError const &err) {
        threw_expected_type = true;

        EXPECT_THAT(err.expect, ElementsAre(LParen, Void, RParen, LBrace));
        EXPECT_THAT(err.got,
                    ElementsAre(TokenMatches(LParen, _, _, _),
                                TokenMatches(LBrace, _, _, _),
                                TokenMatches(Return, _, _, _),
                                TokenMatches(Constant, _, _, "2")));
    }

    EXPECT_TRUE(threw_expected_type);
}

TEST(Parser, EmptySource) {
    using ::jamc::UnexpectedTokenError;
    using enum ::jamc::lex::TokenType;

    std::string_view constexpr source{};
    bool threw_expected_type{false};

    auto lex_res = Lex(source);
    try {
        ParseProgram(lex_res);
    } catch (UnexpectedTokenError const &err) {
        threw_expected_type = true;

        EXPECT_THAT(err.expect, ElementsAre(Int, Ident));
        EXPECT_THAT(err.got, ElementsAre(TokenMatches(Eof, _, _, _)));
    }
    EXPECT_TRUE(threw_expected_type);
}

TEST(Parser, ReturnNonConst) {
    using ::jamc::UnexpectedTokenError;
    using enum ::jamc::lex::TokenType;

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return abcd;\n"
        "}";
    bool threw_expected_type{false};

    auto lex_res = Lex(source);
    try {
        ParseProgram(lex_res);
    } catch (UnexpectedTokenError const &err) {
        threw_expected_type = true;

        EXPECT_THAT(err.expect, ElementsAre(Constant));
        EXPECT_THAT(err.got, ElementsAre(TokenMatches(Ident, _, _, "abcd")));
    }
    EXPECT_TRUE(threw_expected_type);
}

TEST(Parser, ExpectMoreTokensThanIsLeft) {
    using ::jamc::UnexpectedTokenError;
    using enum ::jamc::lex::TokenType;

    std::string_view constexpr source = "int main";
    bool threw_expected_type{false};

    auto lex_res = Lex(source);
    try {
        ParseProgram(lex_res);
    } catch (UnexpectedTokenError const &err) {
        threw_expected_type = true;

        EXPECT_THAT(err.expect, ElementsAre(LParen, Void, RParen, LBrace));
        // We got less tokens than expected!
        EXPECT_THAT(err.got, ElementsAre(TokenMatches(Eof, _, _, _)));
    }
    EXPECT_TRUE(threw_expected_type);
}

TEST(Parser, MinimalSourceWithNoClosingBrace) {
    using ::jamc::UnexpectedTokenError;
    using enum ::jamc::lex::TokenType;

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 2;\n";
    bool threw_expected_type{false};

    auto lex_res = Lex(source);
    try {
        ParseProgram(lex_res);
    } catch (UnexpectedTokenError const &err) {
        threw_expected_type = true;

        EXPECT_THAT(err.expect, ElementsAre(RBrace));
        EXPECT_THAT(err.got, ElementsAre(TokenMatches(Eof, _, _, _)));
    }
    EXPECT_TRUE(threw_expected_type);
}

TEST(Parser, CodeContinuesAfterMinimalSource) {
    using ::jamc::UnexpectedTokenError;
    using enum ::jamc::lex::TokenType;

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 2;\n"
        "}\n"
        "foo";
    bool threw_expected_type{false};

    auto lex_res = Lex(source);
    try {
        ParseProgram(lex_res);
    } catch (UnexpectedTokenError const &err) {
        threw_expected_type = true;

        EXPECT_THAT(err.expect, ElementsAre(Eof));
        EXPECT_THAT(err.got, ElementsAre(TokenMatches(Ident, _, _, "foo")));
    }
    EXPECT_TRUE(threw_expected_type);
}
}  // namespace
