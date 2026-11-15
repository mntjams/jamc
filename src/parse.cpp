#include "jamc/parse.hpp"

#include "jamc/errors.hpp"
#include "jamc/lex.hpp"

#include <charconv>
#include <cstddef>
#include <ranges>
#include <span>
#include <algorithm>

// NOTE - Parsing functions throw on first encountered error.
// In the future, we may want to recover at the next recovery point
// (next ';', '}', etc.) and start parsing again from there.

// TODO - Many functions use the !Expect->throw UnexpectedTokenError.
// This should be abstracted away into a function.

// TODO - The construction of UnexpectedTokenError contains a bug.
// It can point past the end of tokens when we expect N tokens but
// there are fewer than that left.

namespace {
using ::jamc::InvalidConstantError;
using ::jamc::ParsePastTokensEndError;
using ::jamc::UnexpectedTokenError;
using ::jamc::lex::LexResult;
using ::jamc::lex::TokenType;
using ::jamc::parse::Constant;
using ::jamc::parse::Function;
using ::jamc::parse::ReturnExpression;
using ::jamc::parse::Statement;

struct TokenCursor {
public:
    LexResult const &tokens;
    std::size_t idx{0UZ};
};

/**
 * @brief Checks that \p cur.tokens contain \p expect as the next N tokens.
 * On success, updates the index into \p cur to point at
 * the first token past \p expect tokens.
 */
auto Expect(std::vector<TokenType> const &expect, TokenCursor &cur) noexcept
    -> bool {
    namespace rv = std::views;

    // More elems in expect means we are already wrong
    if (expect.size() > cur.tokens.size() - cur.idx) {
        return false;
    }

    for (auto &&p : rv::zip(expect, cur.tokens | rv::drop(cur.idx))) {
        auto const &expect_token_type = std::get<0>(p);
        auto const &actual_token_type = std::get<1>(p).type;
        if (expect_token_type != actual_token_type) {
            return false;
        }
    }

    cur.idx += expect.size();
    return true;
}

auto ParseConstant(TokenCursor &cur) -> Constant {
    auto constexpr const_token = ::jamc::lex::TokenType::Constant;

    if (cur.idx >= cur.tokens.size()) {
        throw ParsePastTokensEndError{};
    }

    auto current_token = cur.tokens[cur.idx];
    if (current_token.type != const_token) {
        throw UnexpectedTokenError{
            {const_token},
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx), std::min(1UZ, cur.tokens.size() - cur.idx)}};
    }

    auto lexeme = cur.tokens[cur.idx].lexeme;
    int val{};

    auto [ptr, ec] =
        std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), val);
    if (ec != std::errc{}) {
        throw InvalidConstantError{lexeme};
    }

    ++cur.idx;
    return Constant{.val = val};
}

auto ParseExpression(TokenCursor &cur) -> ReturnExpression {
    using enum ::jamc::lex::TokenType;

    return ReturnExpression{.constant = ParseConstant(cur)};
}

auto ParseStatement(TokenCursor &cur) -> Statement {
    using enum ::jamc::lex::TokenType;

    std::vector<TokenType> expect{Return};
    if (!Expect(expect, cur)) {
        throw UnexpectedTokenError{
            expect,
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx),
                      std::min(expect.size(), cur.tokens.size() - cur.idx)}};
    }

    auto parsed_expr = ParseExpression(cur);

    expect = {Semic};
    if (!Expect(expect, cur)) {
        throw UnexpectedTokenError{
            expect,
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx),
                      std::min(expect.size(), cur.tokens.size() - cur.idx)}};
    }

    return Statement{.exp = parsed_expr};
}

auto ParseFunction(TokenCursor &cur) -> Function {
    using enum ::jamc::lex::TokenType;

    std::vector<TokenType> expect{Int, Ident};
    if (!Expect(expect, cur)) {
        throw UnexpectedTokenError{
            expect,
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx),
                      std::min(expect.size(), cur.tokens.size() - cur.idx)}};
    }

    // We ended up just past the identifier
    auto name = cur.tokens[cur.idx - 1].lexeme;

    expect = {LParen, Void, RParen, LBrace};
    if (!Expect(expect, cur)) {
        throw UnexpectedTokenError{
            expect,
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx),
                      std::min(expect.size(), cur.tokens.size() - cur.idx)}};
    }

    auto body = ParseStatement(cur);

    expect = {RBrace};
    if (!Expect(expect, cur)) {
        throw UnexpectedTokenError{
            expect,
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx),
                      std::min(expect.size(), cur.tokens.size() - cur.idx)}};
    }

    return Function{.name = name, .body = body};
}
}  // namespace

namespace jamc::parse {
auto ParseProgram(lex::LexResult const &tokens) -> Program {
    using enum ::jamc::lex::TokenType;

    TokenCursor cur{.tokens = tokens};
    auto func = ParseFunction(cur);

    std::vector<TokenType> expect{Eof};
    if (!Expect(expect, cur)) {
        throw UnexpectedTokenError{
            expect,
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx),
                      std::min(expect.size(), cur.tokens.size() - cur.idx)}};
    }

    return Program{.function_definition = func};
}
}  // namespace jamc::parse
