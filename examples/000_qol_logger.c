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

int main() {
    init_logger(.level=LOG_DIAG, .time=true, .color=true); 
    diag("This is a diag message\n");
    info("Starting program with pid=%d\n", 1234);
    exec("cc -o foo foo.c\n");
    hint("Starting program with pid=%d\n", 1234);
    warn("Low memory detected\n");
    erro("Fatal error: %s\n", "out of memory");    // will exit with failure
    dead("Fatal error: %s\n", "out of memory"); // will abort (never reached)

    return EXIT_SUCCESS;
}
