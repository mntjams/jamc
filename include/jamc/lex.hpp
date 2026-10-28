#ifndef JAMC_LEXER_HPP
#define JAMC_LEXER_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace jamc::lex {
struct Token;
enum class TokenType : std::uint8_t;

// Diagnostic is represented as an Unknown token
using LexResult = std::vector<Token>;

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
    Semic,
    Unknown  // Not matching anything
};

struct Token {
public:
    TokenType type{};
    std::size_t line{};
    std::size_t col{};
    // NOTE - This may later be replaced/expanded to support
    // 'interning' (variant of string_view and InternerInfo*)
    // (have one table of identifiers into which we
    // register them and point to it for reference - this allows e.g.
    // faster comparisons - ptr vs ptr instead of string vs string)
    std::string_view lexeme{};
};

auto Lex(std::string_view source) -> LexResult;
}  // namespace jamc::lex

#endif
