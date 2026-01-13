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

    init_logger(.level=LOG_DIAG, .time=false, .color=false); 
    printf("--------------------------\n");
    example(false);

    init_logger(.level=LOG_DIAG, .time=false, .color=true); 
    printf("--------------------------\n");
    example(false);

    init_logger(.level=LOG_DIAG, .time=true, .color=true); 
    printf("--------------------------\n");
    example(false);

    init_logger(.level=LOG_DIAG, .time=true, .color=true, .time_color=true); 
    printf("--------------------------\n");
    example(true);

    return EXIT_SUCCESS;
}
