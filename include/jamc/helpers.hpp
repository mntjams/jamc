#ifndef JAMC_HELPERS_HPP
#define JAMC_HELPERS_HPP

#include "jamc/errors.hpp"

#include <concepts>
#include <expected>
#include <format>
#include <print>
#include <string>
#include <vector>

namespace jamc {
template <typename E>
concept FormatableError =
    std::derived_from<E, Error> && std::formattable<E, char>;

template <FormatableError E>
void ReportErrors(std::vector<E> errs) {
    for (auto i{0UZ}; i < errs.size(); ++i) {
        std::print(stderr, "{}", errs[i]);
        if (i != errs.size() - 1) {
            std::print(stderr, ", ");
        } else {
            std::println(stderr);
        }
    }
}

template <FormatableError E>
void ReportError(E const &err) {
    std::println(stderr, "{}", err);
}

/**
 * @note - We need a string (not string_view) probably
 * because the OS requires null terminated filename
 * (string_view doesn't need to be null terminated).
 */
auto ReadFile(std::string const &filename)
    -> std::expected<std::string, FileReadError>;
}  // namespace jamc

#endif
