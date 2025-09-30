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

#include <stdio.h>

#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_LOGGER
#include "./libaries/000_shl_logger.h"
#include "./libaries/002_shl_no_build.h"

int main() {
    auto_rebuild();

    // TODO: no rebuild of buildconfig if no changes
    // TODO: implement async / parallel execution
    // TODO: make build.c on windows possible
    // TODO: command flags as an array ?

    SystemConfig sysp_1 = { .command = "rm", .command_flags = "-rf out", .cli = true };
    SystemConfig sysp_2 = { .command = "mkdir", .command_flags = "out", .cli = true };

    BuildConfig build_1 = { .source = "example/000_shl_logger.c ", .output = "out/000", .compiler = "gcc" };
    BuildConfig build_2 = { .source = "example/001_shl_cli_arg_parser.c", .output = "out/001", .compiler = "gcc" };
    BuildConfig build_3 = { .source = "example/002_shl_no_build.c", .output = "out/002", .compiler = "gcc" };
    BuildConfig build_4 = { .source = "example/901_shl_demo_calculator.c", .output = "out/901", .compiler = "gcc" };
    BuildConfig build_5 = { .source = "example/902_shl_demo_pointer.c", .output = "out/902", .compiler = "gcc" };

    if (!run(&sysp_1)) { info("Build failed."); }
    if (!run(&sysp_2)) { info("Build failed."); }

    if (!build_project(&build_1)) { info("Build failed."); }
    if (!build_project(&build_2)) { info("Build failed."); }
    if (!build_project(&build_3)) { info("Build failed."); }
    if (!build_project(&build_4)) { info("Build failed."); }
    if (!build_project(&build_5)) { info("Build failed."); }

    return 0;
}
