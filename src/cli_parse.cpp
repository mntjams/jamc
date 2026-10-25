#include "jamc/cli_parse.hpp"

#include "jamc/errors.hpp"

#include <cstddef>
#include <expected>
#include <ranges>
#include <span>
#include <vector>

namespace jamc {
auto CliConfig::Parse(int argc, char const *const *argv) noexcept
    -> ParseResults {
    using enum CliConfigError::ErrorType;
    namespace rg = std::ranges;
    namespace rv = rg::views;

    auto args =
        std::span(argv, static_cast<std::size_t>(argc)) |
        // Drop the executable name
        rv::drop(1) |
        // For each span element (char const *)
        // transform into string_view.
        // the span template arg is tstill 'char const*'
        // but the lazy evaluation transforms it into string_view.
        rv::transform([](char const *arg) { return std::string_view{arg}; });

    CliConfig c{};
    // Use long instead of size_t cuz args indexes with longs
    for (auto i{0L}; i < args.size(); ++i) {
        if (args[i] == "--lex") {
            c.stop_after_lex_ = true;
        } else if (args[i] == "--parse") {
            c.stop_after_parse_ = true;
        } else if (args[i] == "--codegen") {
            c.stop_after_codegen_ = true;
        } else {
            c.num_input_sources_++;
            c.input_source_ = args[i];
        }
    }

    auto validation_res = c.Valid();
    if (validation_res) {
        return c;
    }
    return std::unexpected{validation_res.error()};
}

auto CliConfig::StopAfterLex() const noexcept -> decltype(stop_after_lex_) {
    return stop_after_lex_;
}

auto CliConfig::StopAfterParse() const noexcept -> decltype(stop_after_parse_) {
    return stop_after_parse_;
}

auto CliConfig::StopAfterCodegen() const noexcept
    -> decltype(stop_after_codegen_) {
    return stop_after_codegen_;
}

auto CliConfig::InputSource() const noexcept -> decltype(input_source_) {
    return input_source_;
}

auto CliConfig::Valid() const noexcept -> ValidationResult {
    using enum CliConfigError::ErrorType;

    std::vector<CliConfigError> errs{};

    if (static_cast<std::size_t>(StopAfterLex()) +
            static_cast<std::size_t>(StopAfterParse()) +
            static_cast<std::size_t>(StopAfterCodegen()) >
        1) {
        errs.emplace_back(MultipleStopFlagsError);
    }

    if (num_input_sources_ < 1) {
        errs.emplace_back(NoInputSourceError);
    }

    if (num_input_sources_ > 1) {
        errs.emplace_back(MultipleInputSourcesError);
    }

    if (errs.empty()) {
        return {};
    }

    return std::unexpected{errs};
}
}  // namespace jamc
