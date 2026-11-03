#ifndef JAMC_LEXER_HPP
#define JAMC_LEXER_HPP

#include "jamc/helpers.hpp"

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

namespace std {
template <>
struct formatter<::jamc::lex::Token> {
    template <typename ParseContext>
    auto constexpr parse(ParseContext &ctx) {  // NOLINT
        return ctx.begin();
    }

    template <typename FmtContext>
    auto format(::jamc::lex::Token const &token,  // NOLINT
                FmtContext &ctx) const {
        return format_to(ctx.out(),
                         "\n(type={}, line={}, col={}, lexeme={})",
                         token.type,
                         token.line,
                         token.col,
                         token.lexeme);
    }
};

template <>
struct formatter<::jamc::lex::TokenType> {
    template <typename ParseContext>
    auto constexpr parse(ParseContext &ctx) {  // NOLINT
        return ctx.begin();
    }

    template <typename FmtContext>
    auto format(::jamc::lex::TokenType const &token_type,  // NOLINT
                FmtContext &ctx) const {
        // Typedef required, ADL won't find it because it sits
        // one level above TokenType
        using ::jamc::ToString;
        return format_to(ctx.out(), "{}", ToString(token_type));
    }
};
}  // namespace std
#endif
