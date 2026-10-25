#include "jamc/errors.hpp"

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
