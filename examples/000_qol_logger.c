/*
 * ===========================================================================
 * 000_qol_logger.c
 *
 * Example usage for QOL Logger
 *
 * Created: 29 Sep 2025
 * Author : Raphaele Salvatore Licciardo
 *
 * Copyright (c) 2025 Raphaele Salvatore Licciardo
 * ===========================================================================
 */

#include <stdio.h>

#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "../build.h"

void example(bool with_dead) {
    diag("This is a diag message\n");
    info("Starting program with pid=%d\n", 1234);
    exec("cc -o foo foo.c\n");
    hint("Starting program with pid=%d\n", 1234);
    warn("Low memory detected\n");
    erro("Fatal error: %s\n", "out of memory");
    if (with_dead) dead("Fatal error: %s\n", "out of memory"); // this will exit the program
}

int main() {
    printf("=== Minimum level mode (level=LOG_INFO, shows INFO and above) ===\n");
    init_logger(.level=LOG_INFO, .time=true, .color=true);
    example(false);

    printf("\n=== Only mode (only=LOG_HINT, only_set=true, shows ONLY HINT) ===\n");
    init_logger(.only=LOG_HINT, .only_set=true, .time=true, .color=false);
    example(false);

    printf("\n=== Minimum level mode (level=LOG_DIAG, shows all) ===\n");
    init_logger(.level=LOG_DIAG, .time=true, .color=true, .time_color=true);
    example(false);

    printf("\n=== Minimum level mode (level=LOG_WARN, shows all) ===\n");
    init_logger(.level=LOG_WARN, .time=true, .color=true);
    example(false);

    printf("\n=== Only mode (only=LOG_WARN, only_set=true, shows ONLY WARN) ===\n");
    init_logger(.only=LOG_WARN, .only_set=true, .time=false, .color=true);
    example(false);

    return EXIT_SUCCESS;
}
