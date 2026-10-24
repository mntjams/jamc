#include "jamc/core/errors.hpp"

#include <ostream>

namespace jamc::core {
auto operator<<(std::ostream &ostream, CliConfigError const &err)
    -> std::ostream & {
    return ostream << err.ToString();
}

auto CliConfigError::ToString() const -> std::string_view {
    using enum ErrorType;
    switch (type) {
        case MultipleStopFlagsError:
            return "MultipleStopFlagsError";
        case NoInputSourceError:
            return "NoInputSourceError";
        case MultipleInputSourcesError:
            return "MultipleInputSourcesError";
        default:
            return "UnknownCliConfigErrorType";
    }
}
}  // namespace jamc::core
