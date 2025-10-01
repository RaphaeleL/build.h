/*
 * ===========================================================================
 * 000_shl_logger.c
 *
 * Example usage for SHL Logger
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
#include "../libaries/000_shl_logger.h"

int main() {
    init_logger((LogConfig_t){.level = LOG_DEBUG, .color = true, .time = true});

    debug("This is a debug message\n");
    info("Starting program with pid=%d\n", 1234);
    hint("Starting program with pid=%d\n", 1234);
    warn("Low memory detected\n");
    error("Fatal error: %s\n", "out of memory");
    critical("Fatal error: %s\n", "out of memory");

    return 0;
}
