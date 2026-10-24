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
    template <typename ParseContext>
    auto constexpr parse(ParseContext &ctx) { // NOLINT
        return ctx.begin();
    }

    template <typename FmtContext>
    auto format(::jamc::core::CliConfigError const &err, // NOLINT
                FmtContext &ctx) const {
        return format_to(ctx.out(), "{}", err.ToString());
    }
};
}  // namespace std
#endif
