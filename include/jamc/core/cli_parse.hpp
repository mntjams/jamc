#ifndef CLI_PARSE_HPP
#define CLI_PARSE_HPP

#include "jamc/core/errors.hpp"

#include <cstddef>
#include <expected>
#include <string_view>
#include <vector>

namespace jamc::core {
/**
 * struct CliConfig - Represents the configuration parsed from CLI
 */
struct CliConfig {
private:
    // Declare here so getters can express return values
    // in relation to the types here — this is safer if we change types.
    // Other private members are declared in the next private section.
    bool stop_after_lex_{false};
    bool stop_after_parse_{false};
    bool stop_after_codegen_{false};
    std::string_view input_source_{};

public:
    using ValidationResult = std::expected<void, std::vector<CliConfigError>>;
    using ParseResults = std::expected<CliConfig, std::vector<CliConfigError>>;

    /**
     * @brief Parses the CLI arguments and returns a config on success.
     * Otherwise returns a descriptive error object.
     *
     * @note We need 'char const *const *' type here because
     * 'char const **' is not initializable from 'char **'.
     * (The latter would allow for modification of const values
     * so the C++ standard disallows it).
     */
    static auto Parse(int argc, char const *const *argv) noexcept
        -> ParseResults;

    auto StopAfterLex() const noexcept -> decltype(stop_after_lex_);
    auto StopAfterParse() const noexcept -> decltype(stop_after_parse_);
    auto StopAfterCodegen() const noexcept -> decltype(stop_after_codegen_);
    auto InputSource() const noexcept -> decltype(input_source_);

    /**
     * @brief In case of an error returns std::unexpected
     * with the error type
     */
    auto Valid() const noexcept -> ValidationResult;

private:
    std::size_t num_input_sources_{};

    CliConfig() = default;
};
}  // namespace jamc::core

#endif
