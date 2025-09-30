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
    BuildConfig build_1 = { .source = "example/000_shl_logger.c", .output = "000", .compiler = "gcc" };
    BuildConfig build_2 = { .source = "example/000_shl_logger.c", .output = "000", .compiler = "gcc" };

    if (!build_project(&build_1)) { info("Build failed."); }
    if (!build_project(&build_2)) { info("Build failed."); }

    return 0;
}
