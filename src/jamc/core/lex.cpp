#include "jamc/core/lex.hpp"

#include <string_view>

namespace jamc::core::lex {
auto Lex(std::string_view source) -> LexResult {
    (void)source;
    return LexResult{};
}
}  // namespace jamc::core::lex
