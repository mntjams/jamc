#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <cstdint>
#include <ostream>

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

    friend auto operator<<(std::ostream &ostream, CliConfigError const &err)
        -> std::ostream &;

    auto ToString() const -> std::string_view;
};
}  // namespace jamc::core

#endif
