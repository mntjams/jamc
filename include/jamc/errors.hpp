#ifndef JAMC_ERRORS_HPP
#define JAMC_ERRORS_HPP

#include "jamc/helpers.hpp"

#include <cstdint>
#include <format>
#include <string_view>

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
