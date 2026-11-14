#ifndef JAMC_ERRORS_HPP
#define JAMC_ERRORS_HPP

#include "jamc/helpers.hpp"
#include "jamc/lex.hpp"

#include <cstdint>
#include <format>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace jamc {
struct Error {};

struct CliConfigError : public Error {
public:
    enum class ErrorType : std::uint8_t {
        MultipleStopFlagsError,
        NoInputSourceError,
        MultipleInputSourcesError
    };

    ErrorType type{};

    explicit CliConfigError(ErrorType t)
        : type{t} {};
};

struct FileReadError : public Error {
public:
    enum class ErrorType : std::uint8_t { UnableToOpenFileError };

    ErrorType type{};

    explicit FileReadError(ErrorType t)
        : type{t} {};
};

struct ParseError : public Error {};

struct UnexpectedTokenError : public ParseError {
public:
    std::vector<lex::TokenType> expect;
    std::span<lex::Token const> got;

    UnexpectedTokenError(std::vector<lex::TokenType> expect,
                         std::span<lex::Token const> got)
        : expect{std::move(expect)},
          got{got} {}
};

struct InvalidConstantError : public ParseError {
public:
    std::string_view lexeme{};

    explicit InvalidConstantError(std::string_view lexeme)
        : lexeme{lexeme} {}
};

/**
 * struct ParsePastTokensEndError - Parsing was done past the end of the token
 * input.
 */
struct ParsePastTokensEndError : public ParseError {};
}  // namespace jamc

namespace std {
template <>
struct formatter<::jamc::CliConfigError> {
    template <typename ParseContext>
    auto constexpr parse(ParseContext &ctx) {  // NOLINT
        return ctx.begin();
    }

    template <typename FmtContext>
    auto format(::jamc::CliConfigError const &err,  // NOLINT
                FmtContext &ctx) const {
        return format_to(ctx.out(), "{}", ToString(err.type));
    }
};

template <>
struct formatter<::jamc::FileReadError> {
    template <typename ParseContext>
    auto constexpr parse(ParseContext &ctx) {  // NOLINT
        return ctx.begin();
    }

    template <typename FmtContext>
    auto format(::jamc::FileReadError const &err,  // NOLINT
                FmtContext &ctx) const {
        return format_to(ctx.out(), "{}", ToString(err.type));
    }
};
}  // namespace std
#endif
