#include "jamc/lex.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <string_view>
#include <utility>

namespace {
using ::jamc::lex::Token;
using ::jamc::lex::TokenType;
using enum ::jamc::lex::TokenType;

namespace rg = std::ranges;

/**
 * @brief Stores a sorted table of keywords.
 *
 * @note Works by creating a mutable array and then sorting it
 * but all in a consteval context so the sort becomes consteval.
 */
auto constexpr KeywordTable = [] consteval {
    std::array<std::pair<std::string_view, TokenType>, 4> kw_tbl{
        std::pair{"int", Int},
        std::pair{"void", Void},
        std::pair{"return", Return}};
    rg::sort(kw_tbl, {}, &decltype(kw_tbl)::value_type::first);
    return kw_tbl;
}();
// Guarantee it is sorted
static_assert(rg::is_sorted(KeywordTable,
                            {},
                            &decltype(KeywordTable)::value_type::first));

/**
 * struct Cursor - Cursor into the source code along with current line and col.
 */
struct Cursor {
public:
    std::string_view source{};
    // Position in the source
    std::size_t pos{0};
    std::size_t line{1};
    std::size_t col{1};

    auto AtEnd() const -> bool { return pos == source.length(); }

    auto Peek(std::size_t lookeahed) const -> auto const & {
        return source[pos + lookeahed];
    }

    auto Current() const -> auto const & { return Peek(0); }
};

void ReadWhitespace(Cursor &cursor) {
    if (cursor.Current() == '\n') {
        ++cursor.line;
        // Incremented later to start at 1
        cursor.col = 0;
    }
    ++cursor.col;
    ++cursor.pos;
}

auto IsWord(char c) -> bool { return std::isalnum(c) != 0 || c == '_'; }

/**
 * @brief Is there a word boundary between sv[idx - 1] and sv[idx].
 * Accounts for (idx - 1) or idx being out of range.
 */
auto IsWordBoundary(std::string_view sv, std::size_t idx) -> bool {
    bool const is_prev_word = (idx > 0) && IsWord(sv[idx - 1]);
    bool const is_curr_word = (idx < sv.size()) && IsWord(sv[idx]);
    return is_prev_word != is_curr_word;
}

auto ReadIdentifierOrKeyword(Cursor &cursor) -> Token {
    TokenType type = Ident;
    std::size_t const begin_pos{cursor.pos};
    std::size_t const begin_col{cursor.col};

    // TODO - Every time we inc pos, we also inc col. Can't we merge that?
    ++cursor.pos;
    ++cursor.col;
    for (; !IsWordBoundary(cursor.source, cursor.pos);
         ++cursor.pos, ++cursor.col) {
    }

    auto lexeme = cursor.source.substr(begin_pos, cursor.pos - begin_pos);
    auto const *it = rg::lower_bound(
        KeywordTable, lexeme, {}, &decltype(KeywordTable)::value_type::first);
    if (it != KeywordTable.cend()) {
        type = it->second;
    }

    return Token{
        .type = type, .line = cursor.line, .col = begin_col, .lexeme = lexeme};
}

auto ReadConstant(Cursor &cursor) -> Token {
    // Can change later in the function if lexing fails
    TokenType type{Constant};
    std::size_t const begin_pos{cursor.pos};
    std::size_t const begin_col{cursor.col};

    ++cursor.pos;
    ++cursor.col;

    // Check that everything until the word boundary is a digit
    for (; !IsWordBoundary(cursor.source, cursor.pos);
         ++cursor.pos, ++cursor.col) {
        if (std::isdigit(cursor.Current()) != 0) {
            type = Unknown;
        }
    }

    return Token{.type = type,
                 .line = cursor.line,
                 .col = begin_col,
                 .lexeme = cursor.source.substr(begin_pos, cursor.pos - begin_pos)};
}

/**
 * @brief Reads "( ) { } ;"
 * NOTE - Add other punctuators when their lexing is supported
 */
auto ReadPunctuator(Cursor &cursor) -> Token {
    TokenType type{};
    switch (cursor.Current()) {
        case '(':
            type = LParen;
            break;
        case ')':
            type = RParen;
            break;
        case '{':
            type = LBrace;
            break;
        case '}':
            type = RBrace;
            break;
        case ';':
            type = Semic;
            break;
        default:
            type = Unknown;
            break;
    }
    ++cursor.pos;
    ++cursor.col;
    return Token{.type = type,
                 .line = cursor.line,
                 .col = cursor.col,
                 // The pos is already incremented!
                 .lexeme = cursor.source.substr(cursor.pos - 1, 1)};
}
}  // namespace

namespace jamc::lex {
auto Lex(std::string_view source) -> LexResult {
    LexResult res{};
    Cursor cursor{.source = source};

    while (!cursor.AtEnd()) {
        auto cur_ch = source[cursor.pos];

        if (std::isspace(cur_ch) != 0) {
            ReadWhitespace(cursor);
        } else if (std::isalpha(cur_ch) != 0 || cur_ch == '_') {
            res.emplace_back(ReadIdentifierOrKeyword(cursor));
        } else if (std::isdigit(cur_ch) != 0) {
            res.emplace_back(ReadConstant(cursor));
        } else if (std::ispunct(cur_ch) != 0) {
            res.emplace_back(ReadPunctuator(cursor));
        } else {
            res.emplace_back(Unknown,
                             cursor.line,
                             cursor.col,
                             cursor.source.substr(cursor.pos, 1));
            ++cursor.pos;
            ++cursor.col;
        }
    }
    res.emplace_back(Eof, cursor.line, cursor.col, "");

    return res;
}
}  // namespace jamc::lex
