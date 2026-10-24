#include "jamc/core/cli_parse.hpp"
#include "jamc/core/errors.hpp"

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string_view>

namespace {
using ::jamc::core::CliConfig;
using ::jamc::core::CliConfigError;

using ::testing::Contains;
using ::testing::Field;

using enum ::jamc::core::CliConfigError::ErrorType;

TEST(CliParser, OneSource) {
    std::array<char const *, 2UZ> args{"./jamc", "input.c"};
    auto src_name = std::string_view{args[1]};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_TRUE(parse_res.has_value());

    EXPECT_FALSE(parse_res.value().StopAfterLex());
    EXPECT_FALSE(parse_res.value().StopAfterParse());
    EXPECT_FALSE(parse_res.value().StopAfterCodegen());

    EXPECT_EQ(parse_res.value().InputSource(), src_name);
}

TEST(CliParser, OneSourceStopAfterLex) {
    std::array<char const *, 3UZ> args{"./jamc", "--lex", "input.c"};
    auto src_name = std::string_view{args[2]};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_TRUE(parse_res.has_value());

    EXPECT_TRUE(parse_res.value().StopAfterLex());
    EXPECT_FALSE(parse_res.value().StopAfterParse());
    EXPECT_FALSE(parse_res.value().StopAfterCodegen());

    EXPECT_EQ(parse_res.value().InputSource(), src_name);
}

TEST(CliParser, OneSourceStopAfterParse) {
    std::array<char const *, 3UZ> args{"./jamc", "--parse", "input.c"};
    auto src_name = std::string_view{args[2]};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_TRUE(parse_res.has_value());

    EXPECT_FALSE(parse_res.value().StopAfterLex());
    EXPECT_TRUE(parse_res.value().StopAfterParse());
    EXPECT_FALSE(parse_res.value().StopAfterCodegen());

    EXPECT_EQ(parse_res.value().InputSource(), src_name);
}

TEST(CliParser, OneSourceStopAfterCodegen) {
    std::array<char const *, 3UZ> args{"./jamc", "--codegen", "input.c"};
    auto src_name = std::string_view{args[2]};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_TRUE(parse_res.has_value());

    EXPECT_FALSE(parse_res.value().StopAfterLex());
    EXPECT_FALSE(parse_res.value().StopAfterParse());
    EXPECT_TRUE(parse_res.value().StopAfterCodegen());

    EXPECT_EQ(parse_res.value().InputSource(), src_name);
}

TEST(CliParser, NoArgs) {
    std::array<char const *, 0UZ> args{};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_FALSE(parse_res.has_value());
    EXPECT_THAT(
        parse_res.error(),
        Contains(Field("type", &CliConfigError::type, NoInputSourceError)));
}

TEST(CliParser, MultipleSourceFiles) {
    std::array<char const *, 3UZ> args{"./jamc", "input1.c", "input2.c"};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_FALSE(parse_res.has_value());
    EXPECT_THAT(parse_res.error(),
                Contains(Field(
                    "type", &CliConfigError::type, MultipleInputSourcesError)));
}

TEST(CliParser, OneSourceMultipleStopAfterFlags) {
    std::array<char const *, 4UZ> args{"./jamc", "--lex", "--parse", "input.c"};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_FALSE(parse_res.has_value());
    EXPECT_THAT(
        parse_res.error(),
        Contains(Field("type", &CliConfigError::type, MultipleStopFlagsError)));
}

TEST(CliParser, NoSourceMultipleStopAfterFlags) {
    std::array<char const *, 3UZ> args{"./jamc", "--lex", "--parse"};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_FALSE(parse_res.has_value());
    EXPECT_THAT(
        parse_res.error(),
        Contains(Field("type", &CliConfigError::type, NoInputSourceError)));
    EXPECT_THAT(
        parse_res.error(),
        Contains(Field("type", &CliConfigError::type, MultipleStopFlagsError)));
}

TEST(CliParser, MultipleSourceFilesMultipleStopAfterFlags) {
    std::array<char const *, 5UZ> args{
        "./jamc", "--lex", "--parse", "input1.c", "input2.c"};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_FALSE(parse_res.has_value());
    EXPECT_THAT(parse_res.error(),
                Contains(Field(
                    "type", &CliConfigError::type, MultipleInputSourcesError)));
    EXPECT_THAT(
        parse_res.error(),
        Contains(Field("type", &CliConfigError::type, MultipleStopFlagsError)));
}

TEST(CliParser, NoSourceOneStopAfterFlag) {
    std::array<char const *, 2UZ> args{"./jamc", "--lex"};

    auto parse_res = CliConfig::Parse(args.size(), args.data());

    ASSERT_FALSE(parse_res.has_value());
    EXPECT_THAT(
        parse_res.error(),
        Contains(Field("type", &CliConfigError::type, NoInputSourceError)));
}
}  // namespace
