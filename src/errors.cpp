#include "jamc/errors.hpp"

#include <string_view>

namespace jamc {
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
}  // namespace jamc
