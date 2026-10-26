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
public:
    TokenType type{};
    std::size_t line{};
    std::size_t col{};
};

/**
 * struct IdToken - Holds an identifier
 */
struct IdToken : public Token {
public:
    std::string_view name{};
};

/**
 * struct ConstToken - Holds a constant int value
 */
struct ConstToken : public Token {
public:
    int val{};
};

struct LexResult {
public:
    std::vector<Token> tokens{};
    // TODO - Create vector of diagnostics (warnings/errors)
    //      - One error should have 1. value (name) 2. position
};

auto Lex(std::string_view source) -> LexResult;
}  // namespace jamc::lex

#endif
