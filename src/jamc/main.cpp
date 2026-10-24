#include "jamc/core/cli_parse.hpp"
#include "jamc/core/lex.hpp"
#include "jamc/helpers.hpp"

auto main(int argc, char **argv) -> int {
    using ::jamc::core::CliConfig;
    using ::jamc::ReportErrors;
    using ::jamc::core::lex::Lex;

    auto cli_parse_res{CliConfig::Parse(argc, argv)};
    // The driver script catches these errors already but checking for them
    // here as well (1) won't hurt and (2) can be tested
    if (!cli_parse_res.has_value()) {
        ReportErrors(cli_parse_res.error());
        return 1;
    }

    (void)Lex("");

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
