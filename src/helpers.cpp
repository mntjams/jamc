#include "jamc/helpers.hpp"

#include "jamc/errors.hpp"

#include <filesystem>
#include <fstream>

namespace jamc {
auto ReadFile(std::filesystem::path const &filename)
    -> std::expected<std::string, FileReadError> {
    namespace fs = std::filesystem;
    using enum FileReadError::ErrorType;

    if (!fs::exists(filename)) {
        return std::unexpected{FileReadError{UnableToOpenFileError}};
    }
    auto size = fs::file_size(filename);
    auto istrm = std::ifstream{filename};

    std::string source(size, '\0');
    istrm.read(source.data(), static_cast<std::streamsize>(size));
    return source;
}
}  // namespace jamc
