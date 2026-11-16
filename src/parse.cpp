#include "jamc/parse.hpp"

#include "jamc/errors.hpp"
#include "jamc/lex.hpp"

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <ranges>
#include <span>

// NOTE - Parsing functions throw on first encountered error.
// In the future, we may want to recover at the next recovery point
// (next ';', '}', etc.) and start parsing again from there.

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

/**
 * @brief Call Expect(expect, cur) and throw UnexpectedTokenError if it returns
 * false.
 */
void CheckExpected(std::vector<TokenType> const &expect, TokenCursor &cur) {
    if (!Expect(expect, cur)) {
        throw UnexpectedTokenError{
            expect,
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx),
                      std::min(expect.size(), cur.tokens.size() - cur.idx)}};
    }
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
            std::span{cur.tokens.cbegin() + static_cast<long>(cur.idx),
                      std::min(1UZ, cur.tokens.size() - cur.idx)}};
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

    CheckExpected({Return}, cur);
    auto parsed_expr = ParseExpression(cur);
    CheckExpected({Semic}, cur);
    return Statement{.exp = parsed_expr};
}

auto ParseFunction(TokenCursor &cur) -> Function {
    using enum ::jamc::lex::TokenType;

    CheckExpected({Int, Ident}, cur);
    // We ended up just past the identifier
    auto name = cur.tokens[cur.idx - 1].lexeme;
    CheckExpected({LParen, Void, RParen, LBrace}, cur);
    auto body = ParseStatement(cur);
    CheckExpected({RBrace}, cur);
    return Function{.name = name, .body = body};
}
}  // namespace

namespace jamc::parse {
auto ParseProgram(lex::LexResult const &tokens) -> Program {
    using enum ::jamc::lex::TokenType;

    TokenCursor cur{.tokens = tokens};
    auto func = ParseFunction(cur);
    CheckExpected({Eof}, cur);
    return Program{.function_definition = func};
}
}  // namespace jamc::parse
