/*
 * build.c
 *
 * Custom Build Tool Usage for the SHL Examples
 *
 * Created at:  30. Sep 2025
 * Author:      Raphaele Salvatore Licciardo
 *
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 *
 */

#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_LOGGER
#define SHL_LOG_WITH_COLOR
#include "./libaries/000_shl_logger.h"
#include "./libaries/002_shl_no_build.h"

// TODO: BuildConfig should have a auto_rebuild
// TODO: Make the Shit work on Windows
// TODO: command_flags and compiler_flags should be an array

int main() {
    auto_rebuild();
    init_logger(LOG_INFO);

    SystemConfig sysp_1 = { .command = "rm", .command_flags = "-rf out" };
    SystemConfig sysp_2 = { .command = "mkdir", .command_flags = "out" };

    const char *examples[][2] = {
        { "example/000_shl_logger.c",          "out/000_logger" },
        { "example/001_shl_cli_arg_parser.c",  "out/001_argparser" },
        { "example/002_shl_no_build.c",        "out/002_no_build" },
        { "example/901_shl_demo_calculator.c", "out/901_demo_calculator" },
        { "example/902_shl_demo_pointer.c",    "out/902_demo_pointer" }
    };

    size_t n_examples = sizeof(examples) / sizeof(examples[0]);
    BuildConfig builds[sizeof(examples) / sizeof(examples[0])];

    if (!run(&sysp_1)) { info("Build failed."); }
    if (!run(&sysp_2)) { info("Build failed."); }

    for (size_t i = 0; i < n_examples; i++) {
        builds[i] = (BuildConfig){
            .source   = examples[i][0],
            .output   = examples[i][1],
            .compiler = "gcc",
            .autorun  = false,
            .async    = false
        };
        if (!dispatch_build(&builds[i])) { info("Build failed."); }
    }

    shl_wait_for_all_builds();

    return 0;
}
