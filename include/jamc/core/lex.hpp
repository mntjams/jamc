#ifndef LEXER_HPP
#define LEXER_HPP

#include <string_view>
#include <vector>

namespace jamc::core::lex {
enum class Token {
    // TODO - Create tokens
};

struct LexResult {
   public:
    std::vector<Token> tokens{};
    // TODO - Create vector of diagnostics (warnings/errors)
};

auto Lex(std::string_view source) -> LexResult;
}  // namespace jamc::core::lex

#endif
