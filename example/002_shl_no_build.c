/*
 * 002_shl_no_build.c
 *
 * Example Usage for the SHL No Build Tool
 *
 * Created at:  29. Sep 2025
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
#include "../libaries/002_shl_no_build.h"

int main() {
    BuildConfig config = {
        .source_file = "example/901_shl_demo_calculator.c",
        .output_file = "calc",
        .compiler = "gcc",
        .compiler_flags = "-Wall -Wextra -O2",
        .linker_flags = "-lm"
    };

    if (!build_project(&config)) {
        info("Build failed.");
    }

    return 0;
}
