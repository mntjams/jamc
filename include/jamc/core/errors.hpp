#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <cstdint>
#include <format>
#include <string_view>

namespace jamc::core {
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
        : type(t) {};

    auto ToString() const -> std::string_view;
};
}  // namespace jamc::core

namespace std {
template <>
struct formatter<::jamc::core::CliConfigError> {
    // NOLINTNEXTLINE
    auto constexpr parse(std::format_parse_context &ctx) { return ctx.begin(); }

    // NOLINTNEXTLINE
    auto format(::jamc::core::CliConfigError const &err,
                std::format_context &ctx) const {
        return format_to(ctx.out(), "{}", err.ToString());
    }
};
}  // namespace std
#endif
