#include "jamc/errors.hpp"
#include "jamc/lex.hpp"
#include "jamc/parse.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
using ::jamc::lex::Lex;
using ::jamc::parse::Constant;
using ::jamc::parse::Function;
using ::jamc::parse::ParseProgram;
using ::jamc::parse::Program;
using ::jamc::parse::ReturnExpression;
using ::jamc::parse::Statement;

// TODO - Testing a failed parse only checks that a correct type is thrown.
// Instead, it should do a try/catch and inspect the error type as well
// as its members (if there are any).

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

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 123456789123456789;\n"
        "}";
    auto lex_res = Lex(source);

    EXPECT_THROW(ParseProgram(lex_res), InvalidConstantError);
}

TEST(Parser, BadFunctionDefinition) {
    using ::jamc::UnexpectedTokenError;

    std::string_view constexpr source =
        "int main( {\n"
        "    return 2;\n"
        "}";
    auto lex_res = Lex(source);

    EXPECT_THROW(ParseProgram(lex_res), UnexpectedTokenError);
}

TEST(Parser, EmptySource) {
    using ::jamc::UnexpectedTokenError;

    std::string_view constexpr source{};
    auto lex_res = Lex(source);
    EXPECT_THROW(ParseProgram(lex_res), UnexpectedTokenError);
}

TEST(Parser, ReturnNonConst) {
    using ::jamc::UnexpectedTokenError;

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return abcd;\n"
        "}";
    auto lex_res = Lex(source);
    EXPECT_THROW(ParseProgram(lex_res), UnexpectedTokenError);
}

TEST(Parser, NoConstantAfterReturn) {
    using ::jamc::UnexpectedTokenError;

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return";
    auto lex_res = Lex(source);
    EXPECT_THROW(ParseProgram(lex_res), UnexpectedTokenError);
}

TEST(Parser, MinimalSourceWithNoClosingBrace) {
    using ::jamc::UnexpectedTokenError;

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 2;\n";
    auto lex_res = Lex(source);
    EXPECT_THROW(ParseProgram(lex_res), UnexpectedTokenError);
}

TEST(Parser, CodeContinuesAfterMinimalSource) {
    using ::jamc::UnexpectedTokenError;

    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 2;\n"
        "}\n"
        "foo";
    auto lex_res = Lex(source);
    EXPECT_THROW(ParseProgram(lex_res), UnexpectedTokenError);
}
}  // namespace
