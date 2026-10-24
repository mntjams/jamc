#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <print>
#include <vector>

namespace jamc {
// You could probably restrict E to be a descendant of Error
// but that is too wordy for this trivial usecase
template <typename E>
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
