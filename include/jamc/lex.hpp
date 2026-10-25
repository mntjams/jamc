#ifndef JAMC_LEXER_HPP
#define JAMC_LEXER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace jamc::lex {
enum class TokenType : std::uint8_t {
    Eof,
    Ident,
    Constant,
    Void,
    Int,
    Return,
    LParen,
    RParen,
    LBrace,
    RBrace,
    Semic
};

struct Token {
    TokenType type{};
    std::size_t line{};
    std::size_t col{};
    std::string name{};
};

struct LexResult {
public:
    std::vector<Token> tokens{};
    // TODO - Create vector of diagnostics (warnings/errors)
};

auto Lex(std::string_view source) -> LexResult;
}  // namespace jamc::lex

#endif
