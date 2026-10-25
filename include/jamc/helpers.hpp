#ifndef JAMC_HELPERS_HPP
#define JAMC_HELPERS_HPP

#include "jamc/errors.hpp"

#include <concepts>
#include <format>
#include <print>
#include <vector>

namespace jamc {
template <typename E>
    requires std::derived_from<E, Error> && std::formattable<E, char>
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
}  // namespace jamc

#endif
