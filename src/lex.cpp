#include "jamc/lex.hpp"

#include <string_view>

namespace jamc::lex {
auto Lex(std::string_view source) -> LexResult {
    (void)source;
    return LexResult{};
}
}  // namespace jamc::lex
