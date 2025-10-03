/*
 * ===========================================================================
 * 002_shl_no_build.c
 *
 * Example usage for SHL No Build Tool
 *
 * Created: 29 Sep 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#include <stdio.h>

#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#define SHL_USE_LOGGER
#include "../build.h"

int main() {
    SystemConfig sysp_1 = { .cmd = "rm", .cmd_flags = "-rf out" };
    SystemConfig sysp_2 = { .cmd = "mkdir", .cmd_flags = "out" };

    BuildConfig config = {
        .source = "example/901_shl_demo_calculator.c",
        .output = "out/calc",
        .compiler = "gcc",
        .compiler_flags = "-Wall -Wextra -O2",
        .linker_flags = "-lm"
    };

    if (!command(&sysp_1)) return 1;
    if (!command(&sysp_2)) return 1;
    if (!build_project(&config)) return 1;

    return 0;
}
