#include "jamc/helpers.hpp"

#include "jamc/errors.hpp"

#include <fstream>

namespace jamc {
auto ReadFile(std::string const &filename)
    -> std::expected<std::string, FileReadError> {
    using enum FileReadError::ErrorType;

    auto istrm = std::ifstream{filename};
    if (!istrm.is_open()) {
        return std::unexpected{FileReadError{UnableToOpenFileError}};
    }

    // TODO - Get size with std::filesystem, read it and return
    return std::string{};
}
}  // namespace jamc
