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

TEST(Parser, EmptySource) {
    std::string_view constexpr source{};
    auto lex_res = Lex(source);
    auto parse_res = ParseProgram(lex_res);
    auto expect = Program{};
    EXPECT_EQ(expect, parse_res);
}

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

TEST(Parser, ValueGreaterThanInt) {
    std::string_view constexpr source =
        "int main(void) {\n"
        "    return 123456789123456789;\n"
        "}";
    auto lex_res = Lex(source);
    auto parse_res = ParseProgram(lex_res);

    EXPECT_EQ(/* TODO - What should parser return on failure */, parse_res);
}
}  // namespace
