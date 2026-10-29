#include "jamc/lex.hpp"

#include <cctype>
#include <cstddef>
#include <string_view>

namespace {
using ::jamc::lex::Token;

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
};

void ReadWhitespace(Cursor &cursor) {
    if (cursor.source[cursor.pos] == '\n') {
        ++cursor.line;
        cursor.col = 1;
    }
    ++cursor.col;
    ++cursor.pos;
}

auto ReadIdentifierOrKeyword(Cursor &cursor) -> Token {
    // TODO
    // Identifier [a-zA-Z_]\w*\b
    // int\b
    // void\b
    // return\b
    (void)cursor;
    ++cursor.pos;
    return Token{};
}

auto ReadConstant(Cursor &cursor) -> Token {
    // TODO - Constant [0-9]+\b
    (void)cursor;
    ++cursor.pos;
    return Token{};
}

/**
 * @brief Reads "( ) { } ;"
 * NOTE - Add other punctuators when their lexing is supported
 */
auto ReadPunctuator(Cursor &cursor) -> Token {
    // TODO - Switch on the punctuator type
    // \(
    // \)
    // {
    // }
    // ;
    (void)cursor;
    ++cursor.pos;
    return Token{};
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
            ReadIdentifierOrKeyword(cursor);
        } else if (std::isdigit(cur_ch) != 0) {
            ReadConstant(cursor);
        } else if (std::ispunct(cur_ch) != 0) {
            ReadPunctuator(cursor);
        } else {
            res.emplace_back(TokenType::Unknown, cursor.line, cursor.col, "");
        }
    }

    return res;
}
}  // namespace jamc::lex
