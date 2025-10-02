/*
 * ===========================================================================
 * build.c
 *
 * Custom build tool usage for SHL examples
 *
 * Created: 30 Sep 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_LOGGER
#include "./libaries/000_shl_logger.h"
#include "./libaries/002_shl_no_build.h"

// TODO: BuildConfig should have a auto_rebuild
// TODO: Make the Shit work on Windows
// TODO: command_flags and compiler_flags should be an array

int main() {
    init_logger((LogConfig_t){.level = LOG_INFO, .color = false});
    auto_rebuild();

    SystemConfig sysp_1 = { .command = "rm", .command_flags = "-rf out" };
    SystemConfig sysp_2 = { .command = "mkdir", .command_flags = "out" };

    const char *examples[][2] = {
        { "example/000_shl_logger.c",          "out/000_logger" },
        { "example/001_shl_cli_arg_parser.c",  "out/001_argparser" },
        { "example/002_shl_no_build.c",        "out/002_no_build" },
        { "example/003_shl_dynamic_array.c",   "out/003_dyn_array" },
        { "example/004_shl_helper.c",          "out/004_helper" },
        { "example/901_shl_demo_calculator.c", "out/901_demo_calculator" },
        { "example/902_shl_demo_pointer.c",    "out/902_demo_pointer" }
    };

    size_t n_examples = sizeof(examples) / sizeof(examples[0]);
    BuildConfig builds[sizeof(examples) / sizeof(examples[0])];

    if (!run(&sysp_1)) return 1;
    if (!run(&sysp_2)) return 1;

    for (size_t i = 0; i < n_examples; i++) {
        builds[i] = (BuildConfig){
            .source   = examples[i][0],
            .output   = examples[i][1],
            .compiler = "gcc",
            .autorun  = false,
            .async    = false
        };
        if (!dispatch_build(&builds[i])) return 1;
    }

    wait_for_all_builds();

    return 0;
}
