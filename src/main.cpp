#include "jamc/cli_parse.hpp"
#include "jamc/helpers.hpp"
#include "jamc/lex.hpp"

#include <algorithm>
#include <string>

auto main(int argc, char **argv) -> int {
    using ::jamc::CliConfig;
    using ::jamc::ReadFile;
    using ::jamc::ReportErrors;
    using ::jamc::lex::Lex;

    namespace rg = std::ranges;

    auto cli_parse_res{CliConfig::Parse(argc, argv)};
    // The driver script catches these errors already but checking for them
    // here as well (1) won't hurt and (2) can be tested
    if (!cli_parse_res.has_value()) {
        ReportErrors(cli_parse_res.error());
        return 1;
    }

    auto const input_source =
        ReadFile(std::string{cli_parse_res->InputSource()});
    if (!input_source.has_value()) {
        ReportError(input_source.error());
    }

    auto lex_res = Lex(input_source.value());
    // NOTE - This does additional O(n) work.
    // If this becomes a performance issue in the future,
    // detect failures during lexing (insertion of Unknown token).
    auto unknown_token_it = rg::find(
        lex_res, jamc::lex::TokenType::Unknown, &jamc::lex::Token::type);
    // std::println("{}", lex_res);
    if (unknown_token_it != lex_res.cend()) {
        return 1;
    }
    if (cli_parse_res->StopAfterLex()) {
        return 0;
    }

    // TODO
    // (void)Parse();

    if (cli_parse_res->StopAfterParse()) {
        return 0;
    }

    // TODO
    // (void)CodeGen();

    if (cli_parse_res->StopAfterCodegen()) {
        return 0;
    }

    // TODO
    // (void) EmitAssembly();

    return 0;
}
