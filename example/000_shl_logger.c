/*
 * 000_shl_logger.c
 *
 * Example Usage for the SHL Logger
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
#define SHL_LOG_WITH_TIME
#define SHL_LOG_WITH_COLOR
#include "../libaries/000_shl_logger.h"

// TODO: not sure if SHL_USE_LOGGER, SHL_LOG_WITH_TIME, SHL_LOG_WITH_COLOR
//  should a macro or a setting in init_logger. it could be a struct

int main() {
    init_logger(LOG_DEBUG); // allow everything

    debug("This is a debug message");
    info("Starting program with pid=%d", 1234);
    hint("Starting program with pid=%d", 1234);
    warn("Low memory detected");
    error("Fatal error: %s", "out of memory");
    critical("Fatal error: %s", "out of memory");

    return 0;
}
