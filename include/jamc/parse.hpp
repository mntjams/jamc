#ifndef JAMC_PARSE_HPP
#define JAMC_PARSE_HPP

#include "jamc/lex.hpp"

#include <string_view>

namespace jamc::parse {
struct Constant {
public:
    int val{};
    auto operator<=>(Constant const &other) const noexcept -> bool = default;
};

struct ReturnExpression {
public:
    Constant constant;
    auto operator==(ReturnExpression const &other) const noexcept -> bool = default;
};

struct Statement {
public:
    ReturnExpression exp;
    auto operator==(Statement const &other) const noexcept -> bool = default;
};

struct Function {
public:
    std::string_view name{};
    Statement body;
    auto operator==(Function const &other) const noexcept -> bool = default;
};

struct Program {
public:
    Function function_definition{};
    auto operator==(Program const &other) const noexcept -> bool = default;
};

auto ParseProgram(lex::LexResult const &tokens) -> Program;
}  // namespace jamc::parse

#endif
