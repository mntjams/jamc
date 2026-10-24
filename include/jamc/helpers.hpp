#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <iostream>
#include <vector>

namespace jamc {
// You could probably restrict E to be a descendant of Error
// but that is too wordy for this trivial usecase
template <typename E>
void ReportErrors(std::vector<E> errs) {
    for (auto i{0UZ}; i < errs.size(); ++i) {
        std::cerr << errs[i];
        if (i != errs.size() - 1) {
            std::cerr << ", ";
        } else {
            std::cerr << "\n";
        }
    }
}
}  // namespace jamc

#endif
